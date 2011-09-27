#include "ChatWindowTabsSettings.h"
#include "ui_ChatWindowTabsSettings.h"

#include "SDK/constants.h"
#include "Core.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatWindowTabsSettings::ChatWindowTabsSettings(QWidget *parent): SettingPage(0, parent), m_ui(new Ui::ChatWindowTabsSettings)
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
  Core *core = Core::inst();

  core->setSetting(Settings::S_CHATWINDOW_TABBAR_CLOSE_WND, m_ui->closeWithWindowCheckBox->isChecked());
  core->setSetting(Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, m_ui->hideBarWhenOneTabCheckBox->isChecked());
  core->setSetting(Settings::S_CHATWINDOW_TABBAR_FKEYS, m_ui->fKeysSwitchingCheckBox->isChecked());
  core->setSetting(Settings::S_CHATWINDOW_TABBAR_POS, m_ui->tabBarPositionComboBox->currentIndex());
}

void Kitty::ChatWindowTabsSettings::reset()
{
  Core *core = Core::inst();

  m_ui->closeWithWindowCheckBox->setChecked(core->setting(Settings::S_CHATWINDOW_TABBAR_CLOSE_WND, false).toBool());
  m_ui->hideBarWhenOneTabCheckBox->setChecked(core->setting(Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, true).toBool());
  m_ui->fKeysSwitchingCheckBox->setChecked(core->setting(Settings::S_CHATWINDOW_TABBAR_FKEYS, false).toBool());
  m_ui->tabBarPositionComboBox->setCurrentIndex(core->setting(Settings::S_CHATWINDOW_TABBAR_POS, 0).toInt());
}

void Kitty::ChatWindowTabsSettings::retranslate()
{
  int pos = m_ui->tabBarPositionComboBox->currentIndex();

  m_ui->retranslateUi(this);

  m_ui->tabBarPositionComboBox->setCurrentIndex(pos);
}
