#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "widgets/windows/ChatWindow.h"
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

#include <QtCore/QFile>
#include <QtGui/QToolBar>
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
  connect(chat->contacts().first(), SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SLOT(changeStatus(KittySDK::Protocol::Status, QString)));

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->setStyleSheet("QToolBar { border: 0; }");
  m_ui->verticalLayout->addWidget(m_toolBar);

  Protocol *proto = chat->account()->protocol();

  if(proto->abilities().testFlag(Protocol::TextBold)) {
    QAction *boldAction = m_toolBar->addAction(tr("Bold"));
    boldAction->setProperty("icon_id", Icons::I_BOLD);
  }

  if(proto->abilities().testFlag(Protocol::TextItalics)) {
    QAction *italicAction = m_toolBar->addAction(tr("Italic"));
    italicAction->setProperty("icon_id", Icons::I_ITALIC);
  }

  if(proto->abilities().testFlag(Protocol::TextUnderline)) {
    QAction *underlineAction = m_toolBar->addAction(tr("Underline"));
    underlineAction->setProperty("icon_id", Icons::I_UNDERLINE);
  }

  if(proto->abilities().testFlag(Protocol::TextStrikethrough)) {
    QAction *strikethroughAction = m_toolBar->addAction(tr("Strikethrough"));
    strikethroughAction->setProperty("icon_id", Icons::I_STRIKETHROUGH);
  }

  if(m_toolBar->actions().count() > 0) {
    m_toolBar->addSeparator();
  }

  if(proto->abilities().testFlag(Protocol::TextColor)) {
    QAction *colorAction = m_toolBar->addAction(tr("Color"));
    colorAction->setProperty("icon_id", Icons::I_COLOR);
  }

  QAction *smiliesAction = m_toolBar->addAction(tr("Smilies"));
  smiliesAction->setProperty("icon_id", Icons::I_SMILEY);

  m_toolBar->addSeparator();

  int c = m_toolBar->actions().count();

  if(proto->abilities().testFlag(Protocol::SendImages)) {
    QAction *imageAction = m_toolBar->addAction(tr("Send image"));
    imageAction->setProperty("icon_id", Icons::I_IMAGE);
  }

  if(proto->abilities().testFlag(Protocol::SendFiles)) {
    QAction *fileAction = m_toolBar->addAction(tr("Send file"));
    fileAction->setProperty("icon_id", Icons::I_FILE);
  }

  if(m_toolBar->actions().count() > c) {
    m_toolBar->addSeparator();
  }

  QAction *profileAction = m_toolBar->addAction(tr("Profile"));
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


