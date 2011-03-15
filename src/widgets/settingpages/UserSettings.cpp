#include "UserSettings.h"
#include "ui_UserSettings.h"

#include "SDK/constants.h"

using namespace KittySDK;

Kitty::UserSettings::UserSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::UserSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_USER);
}

Kitty::UserSettings::~UserSettings()
{
  delete m_ui;
}

void Kitty::UserSettings::apply()
{
}

void Kitty::UserSettings::reset()
{
}
