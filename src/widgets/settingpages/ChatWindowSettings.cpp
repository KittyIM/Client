#include "ChatWindowSettings.h"
#include "ui_ChatWindowSettings.h"

#include "SDK/constants.h"

using namespace KittySDK;

Kitty::ChatWindowSettings::ChatWindowSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::ChatWindowSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_MESSAGE);
}

Kitty::ChatWindowSettings::~ChatWindowSettings()
{
  delete m_ui;
}

void Kitty::ChatWindowSettings::apply()
{
}

void Kitty::ChatWindowSettings::reset()
{
}
