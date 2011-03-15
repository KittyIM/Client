#include "ThemesSettings.h"
#include "ui_ThemesSettings.h"

#include "SDK/constants.h"

using namespace KittySDK;

Kitty::ThemesSettings::ThemesSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::ThemesSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_BULLET);
}

Kitty::ThemesSettings::~ThemesSettings()
{
  delete m_ui;
}

void Kitty::ThemesSettings::apply()
{
}

void Kitty::ThemesSettings::reset()
{
}
