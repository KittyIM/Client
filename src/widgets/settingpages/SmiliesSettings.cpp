#include "SmiliesSettings.h"
#include "ui_SmiliesSettings.h"

#include "SDK/constants.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::SmiliesSettings::SmiliesSettings(QWidget *parent): SettingPage(parent), m_ui(new Ui::SmiliesSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_SMILEY);
}

Kitty::SmiliesSettings::~SmiliesSettings()
{
  delete m_ui;
}

void Kitty::SmiliesSettings::apply()
{
}

void Kitty::SmiliesSettings::reset()
{
}
