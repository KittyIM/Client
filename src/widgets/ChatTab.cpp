#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "widgets/windows/MainWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatColorPicker.h"
#include "PluginManager.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Contact.h"
#include "SDK/Message.h"
#include "IconManager.h"
#include "ChatTheme.h"
#include "SDK/Chat.h"
#include "Profile.h"
#include "Core.h"

#ifdef Q_WS_WIN32
#include <qt_windows.h>
#endif

#include <QtCore/QFile>
#include <QtGui/QToolButton>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#define qDebug() qDebug() << "[ChatTab]"
#define qWarning() qWarning() << "[ChatTab]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatTab::ChatTab(Chat *chat, QWidget *parent): QWidget(parent), m_ui(new Ui::ChatTab), m_chat(chat)
{
  m_ui->setupUi(this);

  connect(m_ui->textEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
  connect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateButtons()));
  connect(m_ui->webView, SIGNAL(keyPressed()), m_ui->textEdit, SLOT(setFocus()));
  connect(chat->contacts().first(), SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SLOT(changeStatus(KittySDK::Protocol::Status, QString)));
  connect(chat->contacts().first(), SIGNAL(dataChanged()), this, SIGNAL(tabChanged()));

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->setStyleSheet("QToolBar { border: 0; }");
  m_ui->verticalLayout->addWidget(m_toolBar);

  m_colorPicker = new ChatColorPicker(this);
  connect(m_colorPicker, SIGNAL(colorSelected(QColor)), m_ui->textEdit, SLOT(colorText(QColor)));

  if(chat->contacts().count() == 1) {
    m_ui->treeWidget->hide();
  } else {
    foreach(Contact *cnt, chat->contacts()) {
      QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->treeWidget);
      item->setIcon(0, QIcon(Core::inst()->avatarPath(cnt)));
      item->setText(0, cnt->display());
    }
  }

  Protocol *proto = chat->account()->protocol();

  if(proto->abilities().testFlag(Protocol::TextBold)) {
    m_boldAction = m_toolBar->addAction(tr("Bold"), m_ui->textEdit, SLOT(boldText()));
    m_boldAction->setCheckable(true);
    m_boldAction->setProperty("icon_id", Icons::I_BOLD);
  } else {
    m_boldAction = 0;
  }

  if(proto->abilities().testFlag(Protocol::TextItalics)) {
    m_italicAction = m_toolBar->addAction(tr("Italic"), m_ui->textEdit, SLOT(italicText()));
    m_italicAction->setCheckable(true);
    m_italicAction->setProperty("icon_id", Icons::I_ITALIC);
  } else {
    m_italicAction = 0;
  }

  if(proto->abilities().testFlag(Protocol::TextUnderline)) {
    m_underlineAction = m_toolBar->addAction(tr("Underline"), m_ui->textEdit, SLOT(underlineText()));
    m_underlineAction->setCheckable(true);
    m_underlineAction->setProperty("icon_id", Icons::I_UNDERLINE);
  } else {
    m_underlineAction = 0;
  }

  if(proto->abilities().testFlag(Protocol::TextStrikethrough)) {
    m_strikethroughAction = m_toolBar->addAction(tr("Strikethrough"));
    m_strikethroughAction->setCheckable(true);
    m_strikethroughAction->setProperty("icon_id", Icons::I_STRIKETHROUGH);
  } else {
    m_strikethroughAction = 0;
  }

  if(m_toolBar->actions().count() > 0) {
    m_toolBar->addSeparator();
  }

  if(proto->abilities().testFlag(Protocol::TextColor)) {
    QAction *colorAction = m_toolBar->addAction(tr("Color"), this, SLOT(showColorPicker()));
    colorAction->setProperty("icon_id", Icons::I_COLOR);
  }

  QAction *smiliesAction = m_toolBar->addAction(tr("Smilies"));
  smiliesAction->setProperty("icon_id", Icons::I_SMILEY);

  m_toolBar->addSeparator();

  int c = m_toolBar->actions().count();

  if(proto->abilities().testFlag(Protocol::SendImages)) {
    QAction *imageAction = m_toolBar->addAction(tr("Send image"));
    imageAction->setProperty("icon_id", Icons::I_IMAGE);

    QMenu *imageMenu = new QMenu(this);
    imageMenu->addAction(tr("From file..."));
    imageMenu->addAction(tr("Desktop snapshot"));
    imageMenu->addAction(tr("Desktop snapshot fragment"));

#ifdef Q_WS_WIN32
    QAction *windowAction = imageMenu->addAction(tr("Window snapshot"));

    QMenu *windowMenu = new QMenu(this);

    QList<HWND> visited;

    HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    while(hWnd) {
      if(visited.contains(hWnd)) {
        break;
      }

      visited.append(hWnd);

      if((GetWindowTextLength(hWnd) > 0)) {
        LONG style = GetWindowLong(hWnd, GWL_STYLE);

        if((style & WS_VISIBLE) && (style & WS_CAPTION)) {
          WCHAR *text = new WCHAR[255];
          GetWindowText(hWnd, text, 250);

          QAction *action = windowMenu->addAction(QString::fromWCharArray(text));
          //QPixmap::grabWindow(hWnd).save(QString("C:/%1.png").arg((int)hWnd));

          HICON hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, NULL);
          if(!hIcon) {
            hIcon = (HICON)SendMessage(hWnd, WM_GETICON, 2, NULL);
          }

          if(!hIcon) {
            hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_BIG, NULL);
          }

          if(!hIcon) {
            hIcon = (HICON)GetClassLongPtr(hWnd, GCL_HICON);
          }

          if(!hIcon) {
            hIcon = (HICON)GetClassLongPtr(hWnd, GCL_HICONSM);
          }

          if(hIcon) {
            QPixmap icon = QPixmap::fromWinHICON(hIcon);
            action->setIcon(icon);
          }

          delete text;
          delete hIcon;
        }
      }

      hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }

    windowAction->setMenu(windowMenu);
#endif

    imageMenu->addAction(tr("Clipboard contents"));

    imageAction->setMenu(imageMenu);

    QToolButton *imageButton = qobject_cast<QToolButton*>(m_toolBar->widgetForAction(imageAction));
    imageButton->setPopupMode(QToolButton::InstantPopup);
  }

  if(proto->abilities().testFlag(Protocol::SendFiles)) {
    QAction *fileAction = m_toolBar->addAction(tr("Send file"));
    fileAction->setProperty("icon_id", Icons::I_FILE);
  }

  if(m_toolBar->actions().count() > c) {
    m_toolBar->addSeparator();
  }

  QAction *profileAction = m_toolBar->addAction(tr("vCard"), this, SLOT(showContactWindow()));
  profileAction->setProperty("icon_id", Icons::I_PROFILE);

  QAction *historyAction = m_toolBar->addAction(tr("History"));
  historyAction->setProperty("icon_id", Icons::I_HISTORY);

  updateIcons();

  m_ui->webView->clear();
}

Kitty::ChatTab::~ChatTab()
{
  delete m_ui;
}

void Kitty::ChatTab::updateIcons()
{
  Core *core = Core::inst();

  foreach(QAction *action, m_toolBar->actions()) {
    if(action->property("icon_id").isValid()) {
      action->setIcon(core->icon(action->property("icon_id").toString()));
    }
  }
}

void Kitty::ChatTab::setEditFocus()
{
  m_ui->textEdit->setFocus();
}

void Kitty::ChatTab::applySettings()
{
  m_ui->textEdit->clearHistory();
}

void Kitty::ChatTab::appendMessage(KittySDK::Message &msg)
{
  m_ui->webView->appendMessage(msg);
}

void Kitty::ChatTab::sendMessage()
{
  if(!m_ui->textEdit->toPlainText().isEmpty()) {
    qDebug() << m_ui->textEdit->toHtml().replace("<", "&lt;");

    Message msg(m_chat->me(), m_chat->contacts());
    msg.setBody(m_ui->textEdit->toPlainText());

    QList<Plugin*> plugins = PluginManager::inst()->plugins();
    foreach(Plugin *plugin, plugins) {
      if(plugin->isLoaded()) {
        plugin->plugin()->processMessage(msg);
      }
    }

    m_ui->webView->appendMessage(msg);

    if(Core::inst()->setting(Settings::S_CHATWINDOW_SENTHISTORY, true).toBool()) {
      m_ui->textEdit->addHistory(msg.body());
    }

    m_chat->account()->sendMessage(msg);
    m_ui->textEdit->clear();
  }
}

void Kitty::ChatTab::changeStatus(KittySDK::Protocol::Status status, QString description)
{
  emit tabChanged();
}

void Kitty::ChatTab::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::LanguageChange) {
    m_ui->retranslateUi(this);

    /*m_boldAction->setText(tr("Bold"));
    m_italicAction->setText(tr("Italic"));
    m_underlineAction->setText(tr("Underline"));
    m_colorAction->setText(tr("Color"));
    m_smileyAction->setText(tr("Smilies"));
    m_imageAction->setText(tr("Send image"));
    m_fileAction->setText(tr("Send file"));
    m_profileAction->setText(tr("Profile"));
    m_historyAction->setText(tr("History"));*/
  }

  QWidget::changeEvent(event);
}

void Kitty::ChatTab::updateButtons()
{
  QTextCharFormat fmt = m_ui->textEdit->currentCharFormat();

  if(m_boldAction) {
    m_boldAction->setChecked(fmt.fontWeight() == QFont::Bold);
  }

  if(m_italicAction) {
    m_italicAction->setChecked(fmt.fontItalic());
  }

  if(m_underlineAction) {
    m_underlineAction->setChecked(fmt.fontUnderline());
  }

  if(m_strikethroughAction) {
    m_strikethroughAction->setChecked(fmt.fontStrikeOut());
  }
}

void Kitty::ChatTab::showColorPicker()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QWidget *widget = m_toolBar->widgetForAction(action);
    if(widget) {
      m_colorPicker->showAt(widget->mapToGlobal(QPoint(0, widget->height())));
    }
  }
}

void Kitty::ChatTab::showContactWindow()
{
  Core::inst()->showContactWindow(m_chat->contacts().first());
}
