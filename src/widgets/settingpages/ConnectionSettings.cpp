#include "ConnectionSettings.h"
#include "ui_ConnectionSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

ConnectionSettings::ConnectionSettings(QWidget *parent): KittySDK::ISettingsPage(0, parent), m_ui(new Ui::ConnectionSettings)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_CONNECT);
	setId(KittySDK::SettingPages::S_SETTINGS_CONNECTION);
}

ConnectionSettings::~ConnectionSettings()
{
	delete m_ui;
}

void ConnectionSettings::apply()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_PROXY_ENABLED, m_ui->proxyCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_PROXY_SERVER, m_ui->serverEdit->text());
	core->setSetting(KittySDK::Settings::S_PROXY_PORT, m_ui->portEdit->value());
	core->setSetting(KittySDK::Settings::S_PROXY_AUTH, m_ui->authCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_PROXY_USERNAME, m_ui->usernameEdit->text());
	core->setSetting(KittySDK::Settings::S_PROXY_PASSWORD, m_ui->passwordEdit->text());
}

void ConnectionSettings::reset()
{
	Core *core = Core::inst();

	m_ui->proxyCheckBox->setChecked(core->setting(KittySDK::Settings::S_PROXY_ENABLED).toBool());
	m_ui->serverEdit->setText(core->setting(KittySDK::Settings::S_PROXY_SERVER).toString());
	m_ui->portEdit->setValue(core->setting(KittySDK::Settings::S_PROXY_PORT, 8080).toInt());
	m_ui->authCheckBox->setChecked(core->setting(KittySDK::Settings::S_PROXY_AUTH).toBool());
	m_ui->usernameEdit->setText(core->setting(KittySDK::Settings::S_PROXY_USERNAME).toString());
	m_ui->passwordEdit->setText(core->setting(KittySDK::Settings::S_PROXY_PASSWORD).toString());
}

void ConnectionSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
