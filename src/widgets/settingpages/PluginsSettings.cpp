#include "PluginsSettings.h"
#include "ui_PluginsSettings.h"

#include "SDK/constants.h"

using namespace KittySDK;

Kitty::PluginsSettings::PluginsSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::PluginsSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_PLUGIN);
}

Kitty::PluginsSettings::~PluginsSettings()
{
  delete m_ui;
}

void Kitty::PluginsSettings::apply()
{
}

void Kitty::PluginsSettings::reset()
{
}
