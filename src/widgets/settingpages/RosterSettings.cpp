#include "RosterSettings.h"
#include "ui_RosterSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

RosterSettings::RosterSettings(QWidget *parent): KittySDK::ISettingsPage(0, parent), m_ui(new Ui::RosterSettings)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_BULLET);
	setId(KittySDK::SettingPages::S_DISPLAY_ROSTER);
}

RosterSettings::~RosterSettings()
{
	delete m_ui;
}

void RosterSettings::apply()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_ROSTER_STATUS_DESCRIPTION, m_ui->descriptionComboBox->currentIndex());
	core->setSetting(KittySDK::Settings::S_ROSTER_AVATARS, m_ui->avatarsComboBox->currentIndex());
	core->setSetting(KittySDK::Settings::S_ROSTER_HIDE_OFFLINE, m_ui->hideOfflineContactsCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_GROUPS_AS_TABS, m_ui->groupsAsTabsCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_GROUPS_ON_MAIN, m_ui->groupsOnMainCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_TIPS, m_ui->tipsCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_TIPS_IP, m_ui->displayTipsIPCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_TIPS_EMAIL, m_ui->displayTipsEmailCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_TIPS_PHONE, m_ui->displayTipsPhoneCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_ROSTER_TIPS_DESCRIPTION, m_ui->displayTipsDescriptionCheckBox->isChecked());
}

void RosterSettings::reset()
{
	Core *core = Core::inst();

	m_ui->descriptionComboBox->setCurrentIndex(core->setting(KittySDK::Settings::S_ROSTER_STATUS_DESCRIPTION, 2).toInt());
	m_ui->avatarsComboBox->setCurrentIndex(core->setting(KittySDK::Settings::S_ROSTER_AVATARS, 2).toInt());
	m_ui->hideOfflineContactsCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_HIDE_OFFLINE).toBool());
	m_ui->groupsAsTabsCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_GROUPS_AS_TABS).toBool());
	m_ui->groupsOnMainCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_GROUPS_ON_MAIN).toBool());
	m_ui->tipsCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_TIPS, true).toBool());
	m_ui->displayTipsIPCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_TIPS_IP, true).toBool());
	m_ui->displayTipsEmailCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_TIPS_EMAIL, true).toBool());
	m_ui->displayTipsPhoneCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_TIPS_PHONE, true).toBool());
	m_ui->displayTipsDescriptionCheckBox->setChecked(core->setting(KittySDK::Settings::S_ROSTER_TIPS_DESCRIPTION, true).toBool());
}

void RosterSettings::on_descriptionComboBox_currentIndexChanged(int index)
{
	m_ui->avatarsComboBox->setEnabled(index > 1);
}

void RosterSettings::retranslate()
{
	int description = m_ui->descriptionComboBox->currentIndex();
	int avatar = m_ui->avatarsComboBox->currentIndex();

	m_ui->retranslateUi(this);

	m_ui->descriptionComboBox->setCurrentIndex(description);
	m_ui->avatarsComboBox->setCurrentIndex(avatar);
}

}
