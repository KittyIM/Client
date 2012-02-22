#include "HistorySettings.h"
#include "ui_HistorySettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

HistorySettings::HistorySettings(QWidget *parent): KittySDK::ISettingsPage(0, parent), m_ui(new Ui::HistorySettings)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_HISTORY);
	setId(KittySDK::SettingPages::S_DISPLAY_HISTORY);
}

HistorySettings::~HistorySettings()
{
	delete m_ui;
}

void HistorySettings::apply()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_HISTORY_ENABLED, m_ui->historyCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_HISTORY_STRANGERS, m_ui->saveStrangerHistoryCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_HISTORY_EXPIRE_DAYS, m_ui->expireSpinBox->value());
	core->setSetting(KittySDK::Settings::S_HISTORY_UNDERLINE_LINKS, m_ui->underlineLinksCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_HISTORY_FORMATTING, m_ui->displayFormattingCheckBox->isChecked());
}

void HistorySettings::reset()
{
	Core *core = Core::inst();

	m_ui->historyCheckBox->setChecked(core->setting(KittySDK::Settings::S_HISTORY_ENABLED, true).toBool());
	m_ui->saveStrangerHistoryCheckBox->setChecked(core->setting(KittySDK::Settings::S_HISTORY_STRANGERS, true).toBool());
	m_ui->expireSpinBox->setValue(core->setting(KittySDK::Settings::S_HISTORY_EXPIRE_DAYS, 0).toInt());
	m_ui->underlineLinksCheckBox->setChecked(core->setting(KittySDK::Settings::S_HISTORY_UNDERLINE_LINKS, true).toBool());
	m_ui->displayFormattingCheckBox->setChecked(core->setting(KittySDK::Settings::S_HISTORY_FORMATTING, true).toBool());

}

void HistorySettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
