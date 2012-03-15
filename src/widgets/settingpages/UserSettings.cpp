#include "UserSettings.h"
#include "ui_UserSettings.h"

#include <SDKConstants.h>

namespace Kitty
{

UserSettings::UserSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::UserSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_USER);
	setId(KittySDK::SettingPages::S_USER);
}

UserSettings::~UserSettings()
{
	delete m_ui;
}

void UserSettings::apply()
{
}

void UserSettings::reset()
{
}

void UserSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
