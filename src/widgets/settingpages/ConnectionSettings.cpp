#include "ConnectionSettings.h"
#include "ui_ConnectionSettings.h"

#include "SDK/constants.h"
#include "Core.h"

using namespace KittySDK;

Kitty::ConnectionSettings::ConnectionSettings(QWidget *parent) : KittySDK::SettingPage(parent), m_ui(new Ui::ConnectionSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_CONNECT);
}

Kitty::ConnectionSettings::~ConnectionSettings()
{
  delete m_ui;
}

void Kitty::ConnectionSettings::apply()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_PROXY_ENABLED, m_ui->proxyCheckBox->isChecked());
  core->setSetting(Settings::S_PROXY_SERVER, m_ui->proxyServerEdit->text());
  core->setSetting(Settings::S_PROXY_PORT, m_ui->proxyPortEdit->value());
  core->setSetting(Settings::S_PROXY_AUTH, m_ui->proxyAuthCheckBox->isChecked());
  core->setSetting(Settings::S_PROXY_USERNAME, m_ui->proxyUsernameEdit->text());
  core->setSetting(Settings::S_PROXY_PASSWORD, m_ui->proxyPasswordEdit->text());
}

void Kitty::ConnectionSettings::reset()
{
  Core *core = Core::inst();

  m_ui->proxyCheckBox->setChecked(core->setting(Settings::S_PROXY_ENABLED).toBool());
  m_ui->proxyServerEdit->setText(core->setting(Settings::S_PROXY_SERVER).toString());
  m_ui->proxyPortEdit->setValue(core->setting(Settings::S_PROXY_PORT, 8080).toInt());
  m_ui->proxyAuthCheckBox->setChecked(core->setting(Settings::S_PROXY_AUTH).toBool());
  m_ui->proxyUsernameEdit->setText(core->setting(Settings::S_PROXY_USERNAME).toString());
  m_ui->proxyPasswordEdit->setText(core->setting(Settings::S_PROXY_PASSWORD).toString());
}
