#include "ChatWindowTabsSettings.h"
#include "ui_ChatWindowTabsSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

ChatWindowTabsSettings::ChatWindowTabsSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::ChatWindowTabsSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_TAB);
	setId(KittySDK::SettingPages::S_DISPLAY_CHATWINDOW_TABS);
}

ChatWindowTabsSettings::~ChatWindowTabsSettings()
{
	delete m_ui;
}

void ChatWindowTabsSettings::apply()
{
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_TABBAR_CLOSE_WND, m_ui->closeWithWindowCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, m_ui->hideBarWhenOneTabCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_TABBAR_FKEYS, m_ui->fKeysSwitchingCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_TABBAR_POS, m_ui->tabBarPositionComboBox->currentIndex());
}

void ChatWindowTabsSettings::reset()
{
	m_ui->closeWithWindowCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_CLOSE_WND, false).toBool());
	m_ui->hideBarWhenOneTabCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, true).toBool());
	m_ui->fKeysSwitchingCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_FKEYS, false).toBool());
	m_ui->tabBarPositionComboBox->setCurrentIndex(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_POS, 0).toInt());
}

void ChatWindowTabsSettings::retranslate()
{
	int pos = m_ui->tabBarPositionComboBox->currentIndex();

	m_ui->retranslateUi(this);

	m_ui->tabBarPositionComboBox->setCurrentIndex(pos);
}

}
