#include "ChatWebView.h"

#include "widgets/windows/ChatWindow.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Account.h"
#include "SDK/Contact.h"
#include "SDK/Message.h"
#include "ChatTheme.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
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
  if(request.url().toString() != "about:blank") {
    QDesktopServices::openUrl(request.url());
  }

  return false;
}

Kitty::ChatWebView::ChatWebView(QWidget *parent): QWebView(parent)
{
  m_page = new ChatWebPage();
  setPage(m_page);

  page()->settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
  page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
  page()->settings()->setAttribute(QWebSettings::JavaEnabled, false);
  page()->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
  page()->settings()->setAttribute(QWebSettings::LinksIncludedInFocusChain, false);

  connect(this->page(), SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(handleDownload(QNetworkRequest)));
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void Kitty::ChatWebView::appendMessage(const KittySDK::Message &msg, Kitty::ChatTheme *theme)
{
  Core *core = Core::inst();

  if(!theme) {
    theme = Core::inst()->chatWindow()->theme();
  }

  QString style;

  switch(msg.direction()) {
    case Message::Outgoing:
    {
     style = theme->getCode(ChatTheme::OutgoingContent);
     style.replace("%messageClasses%", "message, outgoing");
    }
    break;

    case Message::Incoming:
    {
     style = theme->getCode(ChatTheme::IncomingContent);
     style.replace("%messageClasses%", "message, incoming");
    }
    break;

    case Message::System:
    {
     style = theme->getCode(ChatTheme::Status);
     style.replace("%messageClasses%", "status");
    }
    break;
  }

  if((msg.direction() == Message::Incoming) || (msg.direction() == Message::Outgoing)) {
    style.replace("%userIconPath%", "");
    style.replace("%senderScreenName%", msg.from()->uid());
    style.replace("%sender%", msg.from()->display());
    style.replace("%senderColor%", "black");
    style.replace("%senderStatusIcon%", "");
    style.replace("%messageDirection%", "ltr");
    style.replace("%senderDisplayName%", "");
    style.replace(QRegExp("%textbackgroundcolor{*}%", Qt::CaseInsensitive, QRegExp::Wildcard), "");
  }

  style.replace("%message%", msg.body());
  style.replace("%time%", msg.timeStamp().toString("hh:mm:ss"));
  style.replace("%shortTime%", msg.timeStamp().toString("hh:mm"));
  style.replace(QRegExp("%time{*}%", Qt::CaseInsensitive, QRegExp::Wildcard), msg.timeStamp().toString("hh:mm:ss"));
  style.replace("%service%", msg.from()->protocol()->protoInfo()->protoName());
  style.replace("%variant%", core->setting(Settings::S_CHAT_THEME_VARIANT, QString()).toString().remove(".css").replace(" ", "_"));
  style.replace("%userIcons%", "showIcons");

  QWebElement elem = page()->mainFrame()->documentElement().findFirst("body");

  if(false) {
    elem = page()->mainFrame()->documentElement().findAll("#insert").last();
  }

  elem.appendInside(style);

  if(page()->mainFrame()->scrollBarValue(Qt::Vertical) == page()->mainFrame()->scrollBarMaximum(Qt::Vertical)) {
    QTimer::singleShot(0, this, SLOT(updateScrollbar()));
  }
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
}

void Kitty::ChatWebView::updateScrollbar()
{
  page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void Kitty::ChatWebView::showContextMenu(QPoint pos)
{
  QMenu menu;

  QWebHitTestResult test = page()->mainFrame()->hitTestContent(pos);
  if(!test.linkUrl().isEmpty()) {
    menu.addAction(pageAction(QWebPage::OpenLink));
    menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
    menu.addSeparator();
  }

  if(!test.imageUrl().isEmpty()) {
    menu.addAction(pageAction(QWebPage::DownloadImageToDisk));
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
  if((event->key() == Qt::Key_C) && (event->modifiers().testFlag(Qt::ControlModifier))) {
    if(!selectedText().isEmpty()) {
      qApp->clipboard()->setText(selectedText());
    }
  }
}

void Kitty::ChatWebView::mouseReleaseEvent(QMouseEvent *event)
{
  Core *core = Core::inst();
 if(core->setting(Settings::S_CHATWINDOW_COPYSELECTION, false).toBool()) {
    if(!selectedText().isEmpty()) {
      qApp->clipboard()->setText(selectedText());

      page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
      page()->mainFrame()->evaluateJavaScript("document.execCommand('unselect');");
      page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    }
  }

  QWebView::mouseReleaseEvent(event);
}
