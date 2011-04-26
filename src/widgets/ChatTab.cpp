#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "PluginManager.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Message.h"
#include "IconManager.h"
#include "SDK/Chat.h"
#include "Profile.h"
#include "Core.h"

#include <QtGui/QToolBar>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

using namespace KittySDK;

Kitty::ChatTab::ChatTab(KittySDK::Chat *chat, QWidget *parent): QWidget(parent), m_ui(new Ui::ChatTab), m_chat(chat)
{
  m_ui->setupUi(this);

  connect(m_ui->textEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

  Kitty::Core *core = Kitty::Core::inst();

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->setStyleSheet("QToolBar { border: 0; }");
  m_ui->verticalLayout->addWidget(m_toolBar);

  KittySDK::Protocol *proto = chat->account()->protocol();

  if(proto->abilities().testFlag(KittySDK::Protocol::TextBold)) {
    QAction *boldAction = m_toolBar->addAction(tr("Bold"));
    boldAction->setProperty("icon_id", Icons::I_BOLD);
  }

  if(proto->abilities().testFlag(KittySDK::Protocol::TextItalics)) {
    QAction *italicAction = m_toolBar->addAction(tr("Italic"));
    italicAction->setProperty("icon_id", Icons::I_ITALIC);
  }

  if(proto->abilities().testFlag(KittySDK::Protocol::TextUnderline)) {
    QAction *underlineAction = m_toolBar->addAction(tr("Underline"));
    underlineAction->setProperty("icon_id", Icons::I_UNDERLINE);
  }

  if(proto->abilities().testFlag(KittySDK::Protocol::TextStriketrough)) {
    QAction *striketroughAction = m_toolBar->addAction(tr("Striketrough"));
    striketroughAction->setProperty("icon_id", Icons::I_STRIKETROUGH);
  }

  if(m_toolBar->actions().count() > 0) {
    m_toolBar->addSeparator();
  }

  if(proto->abilities().testFlag(KittySDK::Protocol::TextColor)) {
    QAction *colorAction = m_toolBar->addAction(tr("Color"));
    colorAction->setProperty("icon_id", Icons::I_COLOR);
  }

  QAction *smiliesAction = m_toolBar->addAction(tr("Smilies"));
  smiliesAction->setProperty("icon_id", Icons::I_SMILEY);

  m_toolBar->addSeparator();

  int c = m_toolBar->actions().count();

  if(proto->abilities().testFlag(KittySDK::Protocol::SendImages)) {
    QAction *imageAction = m_toolBar->addAction(tr("Send image"));
    imageAction->setProperty("icon_id", Icons::I_IMAGE);
  }

  if(proto->abilities().testFlag(KittySDK::Protocol::SendFiles)) {
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
}

Kitty::ChatTab::~ChatTab()
{
  delete m_ui;
}

void Kitty::ChatTab::updateIcons()
{
  Kitty::Core *core = Kitty::Core::inst();

  foreach(QAction *action, m_toolBar->actions()) {
    if(action->property("icon_id").isValid()) {
      action->setIcon(core->icon(action->property("icon_id").toString()));
    }
  }
}

void Kitty::ChatTab::sendMessage()
{
  KittySDK::Message msg(0, m_chat->contacts());
  msg.setBody(m_ui->textEdit->toPlainText());

  m_ui->webView->setHtml(m_ui->webView->page()->mainFrame()->toHtml() + QString("<b>%1:</b><br>%2<br>").arg(Kitty::Core::inst()->profile()->name()).arg(msg.body()));

  QList<Kitty::Plugin*> plugins = Kitty::PluginManager::inst()->plugins();
  foreach(Kitty::Plugin *plugin, plugins) {
    if(plugin->isLoaded()) {
      plugin->plugin()->processMessage(msg);
    }
  }


  m_chat->account()->sendMessage(msg);
  m_ui->textEdit->clear();
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
