#include "HistorySettings.h"
#include "ui_HistorySettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

HistorySettings::HistorySettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::HistorySettings),
	m_core(core)
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
	m_core->setSetting(KittySDK::Settings::S_HISTORY_ENABLED, m_ui->historyCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_HISTORY_STRANGERS, m_ui->saveStrangerHistoryCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_HISTORY_EXPIRE_DAYS, m_ui->expireSpinBox->value());
	m_core->setSetting(KittySDK::Settings::S_HISTORY_UNDERLINE_LINKS, m_ui->underlineLinksCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_HISTORY_FORMATTING, m_ui->displayFormattingCheckBox->isChecked());
}

void HistorySettings::reset()
{
	m_ui->historyCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_HISTORY_ENABLED, true).toBool());
	m_ui->saveStrangerHistoryCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_HISTORY_STRANGERS, true).toBool());
	m_ui->expireSpinBox->setValue(m_core->setting(KittySDK::Settings::S_HISTORY_EXPIRE_DAYS, 0).toInt());
	m_ui->underlineLinksCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_HISTORY_UNDERLINE_LINKS, true).toBool());
	m_ui->displayFormattingCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_HISTORY_FORMATTING, true).toBool());

}

void HistorySettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
