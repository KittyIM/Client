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

  core->setSetting(Settings::S_ROSTER_STATUS_DESCRIPTION, m_ui->descriptionComboBox->currentIndex());
  core->setSetting(Settings::S_ROSTER_AVATARS, m_ui->avatarsComboBox->currentIndex());
  core->setSetting(Settings::S_ROSTER_HIDE_OFFLINE, m_ui->hideOfflineContactsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_AS_TABS, m_ui->groupsAsTabsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_ON_MAIN, m_ui->groupsOnMainCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS, m_ui->tipsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_IP, m_ui->displayTipsIPCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_EMAIL, m_ui->displayTipsEmailCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_PHONE, m_ui->displayTipsPhoneCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_DESCRIPTION, m_ui->displayTipsDescriptionCheckBox->isChecked());
}

void Kitty::RosterSettings::reset()
{
  Core *core = Core::inst();

  m_ui->descriptionComboBox->setCurrentIndex(core->setting(Settings::S_ROSTER_STATUS_DESCRIPTION, 2).toInt());
  m_ui->avatarsComboBox->setCurrentIndex(core->setting(Settings::S_ROSTER_AVATARS, 2).toInt());
  m_ui->hideOfflineContactsCheckBox->setChecked(core->setting(Settings::S_ROSTER_HIDE_OFFLINE).toBool());
  m_ui->groupsAsTabsCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_AS_TABS).toBool());
  m_ui->groupsOnMainCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_ON_MAIN).toBool());
  m_ui->tipsCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS, true).toBool());
  m_ui->displayTipsIPCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_IP).toBool());
  m_ui->displayTipsEmailCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_EMAIL).toBool());
  m_ui->displayTipsPhoneCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_PHONE).toBool());
  m_ui->displayTipsDescriptionCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_DESCRIPTION).toBool());
}

void Kitty::RosterSettings::on_descriptionComboBox_currentIndexChanged(int index)
{
  m_ui->avatarsComboBox->setEnabled(index > 1);
}
