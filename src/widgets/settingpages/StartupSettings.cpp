#include "StartupSettings.h"
#include "ui_StartupSettings.h"

#include <SDKConstants.h>

namespace Kitty
{

StartupSettings::StartupSettings(QWidget *parent): KittySDK::ISettingsPage(0, parent), m_ui(new Ui::StartupSettings)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_BULLET);
	setId(KittySDK::SettingPages::S_SETTINGS_STARTUP);
}

StartupSettings::~StartupSettings()
{
	delete m_ui;
}

void StartupSettings::apply()
{
}

void StartupSettings::reset()
{
}

void StartupSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
