#include "AccountsSettings.h"
#include "ui_AccountsSettings.h"

#include "ProtocolManager.h"
#include "SDK/constants.h"
#include "IconManager.h"

#include <QtCore/QDebug>
#include <QtGui/QMenu>

using namespace KittySDK;

Kitty::AccountsSettings::AccountsSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::AccountsSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_KEY);
}

Kitty::AccountsSettings::~AccountsSettings()
{
  delete m_ui;
}

void Kitty::AccountsSettings::apply()
{
}

void Kitty::AccountsSettings::reset()
{
}

void Kitty::AccountsSettings::on_addButton_clicked()
{

  QMenu menu;

  foreach(KittySDK::Protocol *proto, Kitty::ProtocolManager::inst()->protocols()) {
    menu.addAction(Kitty::IconManager::inst()->icon(KittySDK::Icons::I_IMAGE), proto->info()->name());
  }

  if(menu.isEmpty()) {
    menu.addAction(tr("No protocols available"));
  }

  menu.exec(m_ui->addButton->mapToGlobal(QPoint(0, m_ui->addButton->height())));
}
