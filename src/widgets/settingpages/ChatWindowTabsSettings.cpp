#include "ChatWindowTabsSettings.h"
#include "ui_ChatWindowTabsSettings.h"

#include "SDK/constants.h"

using namespace KittySDK;

Kitty::ChatWindowTabsSettings::ChatWindowTabsSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::ChatWindowTabsSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_TAB);
}

Kitty::ChatWindowTabsSettings::~ChatWindowTabsSettings()
{
  delete m_ui;
}

void Kitty::ChatWindowTabsSettings::apply()
{
}

void Kitty::ChatWindowTabsSettings::reset()
{
}
