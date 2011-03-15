#include "RosterSettings.h"
#include "ui_RosterSettings.h"

#include "SDK/constants.h"
#include "Core.h"

using namespace KittySDK;

Kitty::RosterSettings::RosterSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::RosterSettings)
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

  core->setSetting(Settings::S_ROSTER_STATUS_DESCRIPTION, m_ui->displayRosterDescriptionCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_AVATARS, m_ui->displayRosterAvatarsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_HIDE_OFFLINE, m_ui->hideOfflineContactsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_AS_TABS, m_ui->rosterGroupsAsTabsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_ON_MAIN, m_ui->rosterGroupsOnMainCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_PHONE_ICON, m_ui->rosterPhoneIconCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_EMAIL_ICON, m_ui->rosterEmailIconCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS, m_ui->rosterTipsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_DELAY, m_ui->rosterTipDelaySpinBox->value());
  core->setSetting(Settings::S_ROSTER_TIPS_IP, m_ui->displayRosterTipsIPCheckBox->isChecked());
}

void Kitty::RosterSettings::reset()
{
  Core *core = Core::inst();

  m_ui->displayRosterDescriptionCheckBox->setChecked(core->setting(Settings::S_ROSTER_STATUS_DESCRIPTION, true).toBool());
  m_ui->displayRosterAvatarsCheckBox->setChecked(core->setting(Settings::S_ROSTER_AVATARS, true).toBool());
  m_ui->hideOfflineContactsCheckBox->setChecked(core->setting(Settings::S_ROSTER_HIDE_OFFLINE).toBool());
  m_ui->rosterGroupsAsTabsCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_AS_TABS).toBool());
  m_ui->rosterGroupsOnMainCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_ON_MAIN).toBool());
  m_ui->rosterPhoneIconCheckBox->setChecked(core->setting(Settings::S_ROSTER_PHONE_ICON, true).toBool());
  m_ui->rosterEmailIconCheckBox->setChecked(core->setting(Settings::S_ROSTER_EMAIL_ICON, true).toBool());
  m_ui->rosterTipsCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS, true).toBool());
  m_ui->rosterTipDelaySpinBox->setValue(core->setting(Settings::S_ROSTER_TIPS_DELAY, 250).toInt());
  m_ui->displayRosterTipsIPCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_IP).toBool());
}
