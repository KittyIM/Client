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
  Kitty::Core *core = Kitty::Core::inst();

  core->setSetting(Settings::S_PROXY_ENABLED, m_ui->proxyCheckBox->isChecked());
  core->setSetting(Settings::S_PROXY_SERVER, m_ui->serverEdit->text());
  core->setSetting(Settings::S_PROXY_PORT, m_ui->portEdit->value());
  core->setSetting(Settings::S_PROXY_AUTH, m_ui->authCheckBox->isChecked());
  core->setSetting(Settings::S_PROXY_USERNAME, m_ui->usernameEdit->text());
  core->setSetting(Settings::S_PROXY_PASSWORD, m_ui->passwordEdit->text());
}

void Kitty::ConnectionSettings::reset()
{
  Kitty::Core *core = Kitty::Core::inst();

  m_ui->proxyCheckBox->setChecked(core->setting(Settings::S_PROXY_ENABLED).toBool());
  m_ui->serverEdit->setText(core->setting(Settings::S_PROXY_SERVER).toString());
  m_ui->portEdit->setValue(core->setting(Settings::S_PROXY_PORT, 8080).toInt());
  m_ui->authCheckBox->setChecked(core->setting(Settings::S_PROXY_AUTH).toBool());
  m_ui->usernameEdit->setText(core->setting(Settings::S_PROXY_USERNAME).toString());
  m_ui->passwordEdit->setText(core->setting(Settings::S_PROXY_PASSWORD).toString());
}
