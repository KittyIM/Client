#include "ConnectionSettings.h"
#include "ui_ConnectionSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

ConnectionSettings::ConnectionSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::ConnectionSettings),
	m_core(core)
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
	m_core->setSetting(KittySDK::Settings::S_PROXY_ENABLED, m_ui->proxyCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_PROXY_SERVER, m_ui->serverEdit->text());
	m_core->setSetting(KittySDK::Settings::S_PROXY_PORT, m_ui->portEdit->value());
	m_core->setSetting(KittySDK::Settings::S_PROXY_AUTH, m_ui->authCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_PROXY_USERNAME, m_ui->usernameEdit->text());
	m_core->setSetting(KittySDK::Settings::S_PROXY_PASSWORD, m_ui->passwordEdit->text());
}

void ConnectionSettings::reset()
{
	m_ui->proxyCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_PROXY_ENABLED).toBool());
	m_ui->serverEdit->setText(m_core->setting(KittySDK::Settings::S_PROXY_SERVER).toString());
	m_ui->portEdit->setValue(m_core->setting(KittySDK::Settings::S_PROXY_PORT, 8080).toInt());
	m_ui->authCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_PROXY_AUTH).toBool());
	m_ui->usernameEdit->setText(m_core->setting(KittySDK::Settings::S_PROXY_USERNAME).toString());
	m_ui->passwordEdit->setText(m_core->setting(KittySDK::Settings::S_PROXY_PASSWORD).toString());
}

void ConnectionSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
