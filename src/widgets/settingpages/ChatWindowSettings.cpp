#include "ChatWindowSettings.h"
#include "ui_ChatWindowSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

ChatWindowSettings::ChatWindowSettings(QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::ChatWindowSettings)
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
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_CHATWINDOW_COPYSELECTION, m_ui->copySelectionToClipboardCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_GROUPING, m_ui->groupMessagesCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_SENTHISTORY, m_ui->sentMessagesHistoryCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_UNDERLINE_LINKS, m_ui->underlineMessageLinksCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_STATUS_CHANGES, m_ui->displayStatusChangesCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_YOUTUBE_LINKS, m_ui->displayYouTubeMessagesCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_FORMATTING, m_ui->displayMessageFormattingCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES, m_ui->clearMessagesAmountSpinBox->value());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_CLEAR_INTERVAL, m_ui->clearMinutesAmountSpinBox->value());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_TOOLBAR_POS, m_ui->toolbarPositionComboBox->currentIndex());
}

void ChatWindowSettings::reset()
{
	Core *core = Core::inst();

	m_ui->copySelectionToClipboardCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_COPYSELECTION, false).toBool());
	m_ui->groupMessagesCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_GROUPING, true).toBool());
	m_ui->sentMessagesHistoryCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_SENTHISTORY, true).toBool());
	m_ui->underlineMessageLinksCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_UNDERLINE_LINKS, true).toBool());
	m_ui->displayStatusChangesCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_STATUS_CHANGES, true).toBool());
	m_ui->displayYouTubeMessagesCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_YOUTUBE_LINKS, true).toBool());
	m_ui->displayMessageFormattingCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_FORMATTING, true).toBool());
	m_ui->clearMessagesAmountSpinBox->setValue(core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt());
	m_ui->clearMinutesAmountSpinBox->setValue(core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_INTERVAL).toInt());
	m_ui->toolbarPositionComboBox->setCurrentIndex(core->setting(KittySDK::Settings::S_CHATWINDOW_TOOLBAR_POS, 1).toInt());

}

void ChatWindowSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
