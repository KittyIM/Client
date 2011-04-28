#include "ChatWindow.h"
#include "ui_ChatWindow.h"

#include "widgets/windows/SettingsWindow.h"
#include "SDK/constants.h"
#include "IconManager.h"
#include "SDK/Contact.h"
#include "../ChatTab.h"
#include "ChatTheme.h"
#include "SDK/Chat.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatWindow::ChatWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::ChatWindow)
{
  m_ui->setupUi(this);

  connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));
  connect(Core::inst()->settingsWindow(), SIGNAL(settingsApplied()), this, SLOT(applySettings()));

  qDebug() << "Creating ChatWindow";

  Core *core = Core::inst();

  restoreGeometry(core->setting(Settings::S_CHATWINDOW_GEOMETRY).toByteArray());

  m_theme = 0;

  applySettings();
}

Kitty::ChatWindow::~ChatWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_CHATWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

void Kitty::ChatWindow::applySettings()
{
  if(!m_theme) {
    m_theme = new ChatTheme(Core::inst()->setting(Settings::S_CHAT_THEME).toString(), this);
  } else {
    m_theme->load(Core::inst()->setting(Settings::S_CHAT_THEME).toString());
  }

  for(int i = 0; i < m_ui->tabWidget->count(); i++) {
    ChatTab *tab = qobject_cast<ChatTab*>(m_ui->tabWidget->widget(i));
    tab->applySettings();
  }

  Core *core = Core::inst();

  switch(core->setting(Settings::S_CHATWINDOW_TABBAR_POS).toInt()) {
    case 0:
      m_ui->tabWidget->setTabPosition(QTabWidget::North);
    break;

    case 1:
      m_ui->tabWidget->setTabPosition(QTabWidget::South);
    break;

    case 2:
      m_ui->tabWidget->setTabPosition(QTabWidget::West);
    break;

    case 3:
      m_ui->tabWidget->setTabPosition(QTabWidget::East);
    break;
  }

  m_ui->tabWidget->updateTabBar();
}

void Kitty::ChatWindow::updateIcons()
{
  for(int i = 0; i < m_ui->tabWidget->count(); i++) {
    ChatTab *tab = qobject_cast<ChatTab*>(m_ui->tabWidget->widget(i));
    tab->updateIcons();
  }
}

void Kitty::ChatWindow::startChat(Chat *chat)
{
  ChatTab *chatTab = 0;
  foreach(ChatTab *tab, m_tabs) {
    if(tab->chat() == chat) {
      chatTab = tab;
    }
  }

  if(!chatTab) {
    chatTab = new ChatTab(chat, this);
    m_tabs.append(chatTab);

    int index = m_ui->tabWidget->addTab(chatTab, createLabel(chat));
    m_ui->tabWidget->setCurrentIndex(index);
  } else {
    int index = -1;

    for(int i = 0; i < m_ui->tabWidget->count(); i++) {
      ChatTab *tab = static_cast<ChatTab*>(m_ui->tabWidget->widget(i));
      if(tab->chat() == chat) {
        index = i;
      }
    }

    if(index == -1) {
      int i = m_ui->tabWidget->addTab(chatTab, createLabel(chatTab->chat()));
      m_ui->tabWidget->setCurrentIndex(i);
    } else {
      m_ui->tabWidget->setCurrentIndex(index);
    }
  }
}

void Kitty::ChatWindow::switchTo(Chat *chat)
{
}

void Kitty::ChatWindow::on_tabWidget_tabCloseRequested(int index)
{
  m_ui->tabWidget->removeTab(index);

  if(m_ui->tabWidget->count() == 0) {
    close();
  }
}

QString Kitty::ChatWindow::createLabel(Chat *chat)
{
  QString label = Core::inst()->setting(Settings::S_CHATTAB_CAPTION, "%name%").toString();

  label.replace("%nick%", chat->contacts().first()->display());
  //label.replace("%status%", chat->contacts().first()->status());
  label.replace("%description%", chat->contacts().first()->description());
  label.replace("%unread%", QString::number(0));

  return label;
}

void Kitty::ChatWindow::keyPressEvent(QKeyEvent *event)
{
  Core *core = Core::inst();

  if(core->setting(Settings::S_CHATWINDOW_TABBAR_FKEYS, false).toBool()) {
    if((event->key() >= Qt::Key_F1) && (event->key() <= Qt::Key_F12)) {
      m_ui->tabWidget->setCurrentIndex(event->key() - (int)Qt::Key_F1);
    }
  }

  QWidget::keyPressEvent(event);
}

void Kitty::ChatWindow::closeEvent(QCloseEvent *event)
{
  Core *core = Core::inst();

  if(core->setting(Settings::S_CHATWINDOW_TABBAR_CLOSE_WND, false).toBool()) {
    for(int i = m_ui->tabWidget->count() - 1; i >= 0; i--) {
      m_ui->tabWidget->removeTab(i);
    }
  }
}
