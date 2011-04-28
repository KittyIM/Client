#include "StartupSettings.h"
#include "ui_StartupSettings.h"

#include "SDK/constants.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::StartupSettings::StartupSettings(QWidget *parent): SettingPage(parent), m_ui(new Ui::StartupSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_BULLET);
}

Kitty::StartupSettings::~StartupSettings()
{
  delete m_ui;
}

void Kitty::StartupSettings::apply()
{
}

void Kitty::StartupSettings::reset()
{
}
