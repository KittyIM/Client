#include "RosterSettings.h"
#include "ui_RosterSettings.h"

#include "SDK/constants.h"
#include "Core.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::RosterSettings::RosterSettings(QWidget *parent): SettingPage(parent), m_ui(new Ui::RosterSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_BULLET);
}

Kitty::RosterSettings::~RosterSettings()
{
  delete m_ui;
}

void Kitty::RosterSettings::apply()
{
  Core *core = Core::inst();

  //core->setSetting(Settings::S_ROSTER_STATUS_DESCRIPTION, m_ui->displayDescriptionCheckBox->isChecked());
  //core->setSetting(Settings::S_ROSTER_AVATARS, m_ui->displayAvatarsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_HIDE_OFFLINE, m_ui->hideOfflineContactsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_AS_TABS, m_ui->groupsAsTabsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_ON_MAIN, m_ui->groupsOnMainCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS, m_ui->tipsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_DELAY, m_ui->tipDelaySpinBox->value());
  core->setSetting(Settings::S_ROSTER_TIPS_IP, m_ui->displayTipsIPCheckBox->isChecked());
}

void Kitty::RosterSettings::reset()
{
  Core *core = Core::inst();

  //m_ui->displayDescriptionCheckBox->setChecked(core->setting(Settings::S_ROSTER_STATUS_DESCRIPTION, true).toBool());
  //m_ui->displayAvatarsCheckBox->setChecked(core->setting(Settings::S_ROSTER_AVATARS, true).toBool());
  m_ui->hideOfflineContactsCheckBox->setChecked(core->setting(Settings::S_ROSTER_HIDE_OFFLINE).toBool());
  m_ui->groupsAsTabsCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_AS_TABS).toBool());
  m_ui->groupsOnMainCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_ON_MAIN).toBool());
  m_ui->tipsCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS, true).toBool());
  m_ui->tipDelaySpinBox->setValue(core->setting(Settings::S_ROSTER_TIPS_DELAY, 250).toInt());
  m_ui->displayTipsIPCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_IP).toBool());
}
