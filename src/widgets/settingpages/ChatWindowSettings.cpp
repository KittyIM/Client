#include "ChatWindowSettings.h"
#include "ui_ChatWindowSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

ChatWindowSettings::ChatWindowSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::ChatWindowSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_MESSAGE);
	setId(KittySDK::SettingPages::S_DISPLAY_CHATWINDOW);
}

ChatWindowSettings::~ChatWindowSettings()
{
	delete m_ui;
}

void ChatWindowSettings::apply()
{
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_COPYSELECTION, m_ui->copySelectionToClipboardCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_GROUPING, m_ui->groupMessagesCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_SENTHISTORY, m_ui->sentMessagesHistoryCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_UNDERLINE_LINKS, m_ui->underlineMessageLinksCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_STATUS_CHANGES, m_ui->displayStatusChangesCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_YOUTUBE_LINKS, m_ui->displayYouTubeMessagesCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_FORMATTING, m_ui->displayMessageFormattingCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES, m_ui->clearMessagesAmountSpinBox->value());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_CLEAR_INTERVAL, m_ui->clearMinutesAmountSpinBox->value());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_TOOLBAR_POS, m_ui->toolbarPositionComboBox->currentIndex());
}

void ChatWindowSettings::reset()
{
	m_ui->copySelectionToClipboardCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_COPYSELECTION, false).toBool());
	m_ui->groupMessagesCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_GROUPING, true).toBool());
	m_ui->sentMessagesHistoryCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_SENTHISTORY, true).toBool());
	m_ui->underlineMessageLinksCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_UNDERLINE_LINKS, true).toBool());
	m_ui->displayStatusChangesCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_STATUS_CHANGES, true).toBool());
	m_ui->displayYouTubeMessagesCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_YOUTUBE_LINKS, true).toBool());
	m_ui->displayMessageFormattingCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_FORMATTING, true).toBool());
	m_ui->clearMessagesAmountSpinBox->setValue(m_core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt());
	m_ui->clearMinutesAmountSpinBox->setValue(m_core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_INTERVAL).toInt());
	m_ui->toolbarPositionComboBox->setCurrentIndex(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TOOLBAR_POS, 1).toInt());
}

void ChatWindowSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
