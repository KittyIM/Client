#include "AccountsSettings.h"
#include "ui_AccountsSettings.h"

#include "SDK/constants.h"

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
