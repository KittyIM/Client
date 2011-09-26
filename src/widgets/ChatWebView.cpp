#include "ChatWebView.h"

#include "widgets/windows/ChatWindow.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Account.h"
#include "SDK/Contact.h"
#include "ChatTheme.h"
#include "Core.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtGui/QDesktopServices>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>

#define qDebug() qDebug() << "[ChatWebPage]"
#define qWarning() qWarning() << "[ChatWebPage]"

using namespace Kitty;
using namespace KittySDK;

bool Kitty::ChatWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type)
{
  QString scheme = request.url().scheme();

  if(scheme == "kitty") {
    QString command = request.url().toString().mid(6);

    if(command.left(3) == "img") {
      QWebElement div = mainFrame()->documentElement().findFirst(QString("#%1").arg(command));

      if(div.attribute("style") == "display: none;") {
        div.setAttribute("style", "display: block;");
      } else {
        div.setAttribute("style", "display: none;");
      }
    }
  } else if(scheme != "about") {
    QDesktopServices::openUrl(request.url());
  }

  return false;
}

Kitty::ChatWebView::ChatWebView(QWidget *parent): QWebView(parent)
{
  m_page = new ChatWebPage(this);
  setPage(m_page);

  m_imageCount = 0;
  m_lastFrom = 0;

  page()->settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
  page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
  page()->settings()->setAttribute(QWebSettings::JavaEnabled, false);
  page()->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
  page()->settings()->setAttribute(QWebSettings::LinksIncludedInFocusChain, false);

  connect(this->page(), SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(handleDownload(QNetworkRequest)));
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

Kitty::ChatWebView::~ChatWebView()
{
  delete m_page;
}

void Kitty::ChatWebView::appendMessage(const KittySDK::Message &msg, Kitty::ChatTheme *theme)
{
  Core *core = Core::inst();

  if(!theme) {
    theme = Core::inst()->chatWindow()->theme();
  }

  //let's if this is a consecutive message (sent by same side within 5 minutes)
  bool next = false;
  if(core->setting(Settings::S_CHATWINDOW_GROUPING, true).toBool()) {
    if(m_lastFrom) {
      if((m_lastFrom == msg.from()) && (m_lastTimeStamp.secsTo(msg.timeStamp()) <= 300) && (m_lastDirection == msg.direction())) {
        next = true;
      }
    }
  }

  QString style;
  switch(msg.direction()) {
    case Message::Outgoing:
    {
      if(next) {
        style = theme->code(ChatTheme::OutgoingNextContent);
        style.replace("%messageClasses%", "message, outgoing, consecutive");
      } else {
        style = theme->code(ChatTheme::OutgoingContent);
        style.replace("%messageClasses%", "message, outgoing");
      }
    }
    break;

    case Message::Incoming:
    {
      if(next) {
        style = theme->code(ChatTheme::IncomingNextContent);
        style.replace("%messageClasses%", "message, incoming, consecutive");
      } else {
        style = theme->code(ChatTheme::IncomingContent);
        style.replace("%messageClasses%", "message, incoming");
      }
    }
    break;

    case Message::System:
    {
      style = theme->code(ChatTheme::Status);
      style.replace("%messageClasses%", "status");
    }
    break;
  }

  if((msg.direction() == Message::Incoming) || (msg.direction() == Message::Outgoing)) {
    if(msg.direction() == Message::Incoming) {
      QString avatar = core->avatarPath(msg.from());
      if(!QFile::exists(avatar)) {
        avatar = theme->iconPath(ChatTheme::Incoming);
      }
      style.replace("%userIconPath%", avatar);
    } else {
      QString avatar = core->currentProfileDir() + "avatar.png";
      if(!QFile::exists(avatar)) {
        avatar = theme->iconPath(ChatTheme::Outgoing);
      }
      style.replace("%userIconPath%", avatar);
    }

    style.replace("%senderScreenName%", msg.from()->uid());
    style.replace("%sender%", msg.from()->display());
    style.replace("%senderColor%", "black");
    style.replace("%senderStatusIcon%", "");
    style.replace("%messageDirection%", "ltr");
    style.replace("%senderDisplayName%", "");
    style.replace(QRegExp("%textbackgroundcolor{*}%", Qt::CaseInsensitive, QRegExp::Wildcard), "");
  }

  QString body = msg.body();
  if(core->setting(Settings::S_CHATWINDOW_UNDERLINE_LINKS, true).toBool()) {
    body = core->processUrls(body);
  }

  if(!core->setting(Settings::S_CHATWINDOW_FORMATTING, true).toBool()) {
    body.remove(QRegExp("<[^>]*>"));
  }

  style.replace("%message%", body);
  style.replace("%time%", msg.timeStamp().toString("hh:mm:ss"));
  style.replace("%shortTime%", msg.timeStamp().toString("hh:mm"));
  style.replace(QRegExp("%time{*}%", Qt::CaseInsensitive, QRegExp::Wildcard), msg.timeStamp().toString("hh:mm:ss"));

  if(msg.from()->protocol()) {
    style.replace("%service%", msg.from()->protocol()->protoInfo()->protoName());
  }

  style.replace("%variant%", core->setting(Settings::S_CHAT_THEME_VARIANT, QString()).toString().remove(".css").replace(" ", "_"));
  style.replace("%userIcons%", "showIcons");

  QWebElement elem = page()->mainFrame()->documentElement().findFirst("body");

  if(next) {
    QWebElement last = page()->mainFrame()->documentElement().findAll("#insert").last();
    if(!last.isNull()) {
      elem = last;
    }
  }

  elem.appendInside(style);

  if(msg.direction() == Message::Incoming) {
    QString images = findImages(body, theme);
    if(!images.isEmpty()) {
      elem.appendInside(images);
    }

    if(core->setting(Settings::S_CHATWINDOW_YOUTUBE_LINKS, true).toBool()) {
      QString youtubes = findYoutubes(body, theme);
      if(!youtubes.isEmpty()) {
        elem.appendInside(youtubes);
      }
    }
  }

  //without the timer, the scollbars wouldn't update
  if(page()->mainFrame()->scrollBarValue(Qt::Vertical) == page()->mainFrame()->scrollBarMaximum(Qt::Vertical)) {
    QTimer::singleShot(0, this, SLOT(updateScrollbar()));
  }

  m_lastFrom = msg.from();
  m_lastTimeStamp = msg.timeStamp();
  m_lastDirection = msg.direction();
}

void Kitty::ChatWebView::clear()
{
  clearTo();
}

void Kitty::ChatWebView::clearTo(bool custom, const QString &theme, const QString &variant)
{
  Core *core = Core::inst();

  QString style = "Main.css";
  if(custom) {
    if(!variant.isEmpty()) {
      style = "Variants/" + variant;
    }
  } else {
    if(!core->setting(Settings::S_CHAT_THEME_VARIANT).toString().isEmpty()) {
      style = "Variants/" + core->setting(Settings::S_CHAT_THEME_VARIANT).toString();
    }
  }

  QString path = "qrc:/chat";
  if(custom) {
    if(!theme.isEmpty()) {
      path = qApp->applicationDirPath() + "/themes/chat/" + theme;
    }
  } else {
    if(!core->setting(Settings::S_CHAT_THEME).toString().isEmpty()) {
      path = qApp->applicationDirPath() + "/themes/chat/" + core->setting(Settings::S_CHAT_THEME).toString();
    }
  }

  QString html;
  html += "<html>";
  html += "<head>";
  html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"" + path + "/" + style + "\">";
  html += "<style type=\"text/css\"> body { font-size: 12px; margin: 2px; } </style>";
  html += "</head>";
  html += "<body>";
  html += "</body>";

  setHtml(html);

  m_lastFrom = 0;
}

void Kitty::ChatWebView::updateScrollbar()
{
  page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void Kitty::ChatWebView::showContextMenu(QPoint pos)
{
  QMenu menu;

  QWebHitTestResult test = page()->mainFrame()->hitTestContent(pos);

  //if clicked on a link
  if(!test.linkUrl().isEmpty()) {
    menu.addAction(pageAction(QWebPage::OpenLink));
    menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
    menu.addSeparator();
  }

  //if clicked on image
  if(!test.imageUrl().isEmpty()) {
    menu.addAction(pageAction(QWebPage::DownloadImageToDisk));
    menu.addAction(pageAction(QWebPage::CopyImageToClipboard));
    menu.addSeparator();
  }

  menu.addAction(pageAction(QWebPage::Copy));
  menu.addAction(pageAction(QWebPage::SelectAll));
  menu.addSeparator();
  menu.addAction(tr("Clear"), this, SLOT(clear()));
  menu.exec(mapToGlobal(pos));
}

void Kitty::ChatWebView::handleDownload(QNetworkRequest req)
{
  qDebug() << "Download requested:" << req.url();
}

void Kitty::ChatWebView::keyPressEvent(QKeyEvent *event)
{
  if(event->modifiers().testFlag(Qt::ControlModifier)) {
    if(event->key() == Qt::Key_C) {
      if(!selectedText().isEmpty()) {
        qApp->clipboard()->setText(selectedText());
      }
    }
  } else {
    emit keyPressed();
  }
}

void Kitty::ChatWebView::mouseReleaseEvent(QMouseEvent *event)
{
  Core *core = Core::inst();
 if(core->setting(Settings::S_CHATWINDOW_COPYSELECTION, false).toBool()) {
    if(!selectedText().isEmpty()) {
      qApp->clipboard()->setText(selectedText());

      //clear selection
      page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
      page()->mainFrame()->evaluateJavaScript("document.execCommand('unselect');");
      page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    }
  }

 QWebView::mouseReleaseEvent(event);
}

QString Kitty::ChatWebView::findImages(const QString &body, Kitty::ChatTheme *theme)
{
  QString result;

  //load Status theme style
  QString style = theme->code(ChatTheme::Status);
  style.replace("%messageClasses%", "status");
  style.replace("%time%", "");
  style.replace("%shortTime%", "");
  style.replace(QRegExp("%time{*}%", Qt::CaseInsensitive, QRegExp::Wildcard), "");
  style.replace("%service%", "");
  style.replace("%variant%", Core::inst()->setting(Settings::S_CHAT_THEME_VARIANT, QString()).toString().remove(".css").replace(" ", "_"));
  style.replace("%userIcons%", "showIcons");

  QStringList added;
  QRegExp imgs("http://[-A-Z0-9+&@#/%?=~_|$!:,.;]*\\.(png|gif|jpeg|jpg|tiff|bmp)", Qt::CaseInsensitive);
  int pos = 0;
  while((pos = imgs.indexIn(body, pos)) != -1) {
    QString url = imgs.cap(0);

    //add only once
    if(!added.contains(url)) {
      QString code = style;
      code.replace("%message%", QString("<a href=\"kitty:img%2\">%3</a><div id=\"img%2\" style=\"display: none;\"><img src=\"%1\" style=\"max-width: 100%;\"></div>").arg(url).arg(m_imageCount).arg(QFileInfo(url).fileName()));
      result.append(code);
      added << imgs.cap(0);
    }

    pos += imgs.matchedLength();
    m_imageCount++;
  }

  return result;
}

QString Kitty::ChatWebView::findYoutubes(const QString &body, Kitty::ChatTheme *theme)
{
  QString result;

  //load Status theme style
  QString style = theme->code(ChatTheme::Status);
  style.replace("%messageClasses%", "status");
  style.replace("%time%", "");
  style.replace("%shortTime%", "");
  style.replace(QRegExp("%time{*}%", Qt::CaseInsensitive, QRegExp::Wildcard), "");
  style.replace("%service%", "");
  style.replace("%variant%", Core::inst()->setting(Settings::S_CHAT_THEME_VARIANT, QString()).toString().remove(".css").replace(" ", "_"));
  style.replace("%userIcons%", "showIcons");

  QStringList added;
  QRegExp youtubes("http://\\w{0,3}.?youtube+\\.\\w{2,3}/watch\\?(?:[A-z0-9]*=[A-z0-9]*&)*v=([\\w-]{11})", Qt::CaseInsensitive);
  int pos = 0;
  while((pos = youtubes.indexIn(body, pos)) != -1) {
    QString url = youtubes.cap(0);
    QString id = youtubes.cap(1);

    //add only once
    if(!added.contains(url)) {
      QString code = style;

      // TODO
      // Display movie title instead of url, source:
      // http://gdata.youtube.com/feeds/api/videos/[id]?alt=json
      code.replace("%message%", QString("<a href=\"kitty:img%2\">%1</a><div id=\"img%2\" style=\"display: none;\"><object type=\"application/x-shockwave-flash\" style=\"width:450px; height:366px;\" data=\"http://www.youtube.com/v/%3?rel=0&fs=1\"><param name=\"movie\" value=\"http://www.youtube.com/v/%3?rel=0&fs=1\"><param name=\"allowFullScreen\" value=\"true\"></object></div>").arg(url).arg(m_imageCount).arg(id));
      result.append(code);
      added << youtubes.cap(0);
    }

    pos += youtubes.matchedLength();
    m_imageCount++;
  }

  return result;
}
