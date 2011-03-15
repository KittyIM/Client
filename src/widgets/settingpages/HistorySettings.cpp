#include "HistorySettings.h"
#include "ui_HistorySettings.h"

#include "SDK/constants.h"

using namespace KittySDK;

Kitty::HistorySettings::HistorySettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::HistorySettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_HISTORY);
}

Kitty::HistorySettings::~HistorySettings()
{
  delete m_ui;
}

void Kitty::HistorySettings::apply()
{
}

void Kitty::HistorySettings::reset()
{
}
