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
    m_theme = new Kitty::ChatTheme(Core::inst()->setting(Settings::S_CHAT_THEME).toString(), this);
  } else {
    m_theme->load(Core::inst()->setting(Settings::S_CHAT_THEME).toString());
  }

  for(int i = 0; i < m_ui->tabWidget->count(); i++) {
    Kitty::ChatTab *tab = qobject_cast<Kitty::ChatTab*>(m_ui->tabWidget->widget(i));
    tab->applySettings();
  }
}

void Kitty::ChatWindow::updateIcons()
{
  for(int i = 0; i < m_ui->tabWidget->count(); i++) {
    Kitty::ChatTab *tab = qobject_cast<Kitty::ChatTab*>(m_ui->tabWidget->widget(i));
    tab->updateIcons();
  }
}

void Kitty::ChatWindow::startChat(KittySDK::Chat *chat)
{
  QString label = Kitty::Core::inst()->setting(Settings::S_CHATTAB_CAPTION, "%name%").toString();
  label = label.replace("%nick%", chat->contacts().first()->display());
  //label = label.replace("%status%", chat->contacts().first()->status());
  label = label.replace("%description%", chat->contacts().first()->description());
  label = label.replace("%unread%", QString::number(0));

  int tab = m_ui->tabWidget->addTab(new Kitty::ChatTab(chat, this), label);
  m_ui->tabWidget->setCurrentIndex(tab);

}

void Kitty::ChatWindow::switchTo(KittySDK::Chat *chat)
{
  m_ui->tabWidget->switchTo(chat);
}

void Kitty::ChatWindow::on_tabWidget_tabCloseRequested(int index)
{
  delete m_ui->tabWidget->widget(index);
}
