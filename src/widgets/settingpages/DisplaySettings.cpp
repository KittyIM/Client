#include "DisplaySettings.h"
#include "ui_DisplaySettings.h"

#include "SDK/constants.h"
#include "Core.h"

using namespace KittySDK;

Kitty::DisplaySettings::DisplaySettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::DisplaySettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_PALETTE);
}

Kitty::DisplaySettings::~DisplaySettings()
{
  delete m_ui;
}

void Kitty::DisplaySettings::apply()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_MAINWINDOW_ALWAYS_ON_TOP, m_ui->alwaysOnTopCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_AUTOHIDE, m_ui->autoHideCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY, m_ui->autoHideSlider->value());
  core->setSetting(Settings::S_MAINWINDOW_TRANSPARENCY, m_ui->transparencyCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, m_ui->transparencySlider->value());
  core->setSetting(Settings::S_MAINWINDOW_DOCKING, m_ui->dockToEdgesCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_DOCKING_DISTANCE, m_ui->dockDistanceSpinBox->value());
  core->setSetting(Settings::S_MAINWINDOW_CAPTION, m_ui->mainWindowCaptionEdit->text());
  core->setSetting(Settings::S_CHATWINDOW_CAPTION, m_ui->chatWindowCaptionEdit->text());
  core->setSetting(Settings::S_CHATTAB_CAPTION, m_ui->chatTabCaptionEdit->text());
}

void Kitty::DisplaySettings::reset()
{
  Core *core = Core::inst();

  m_ui->alwaysOnTopCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_ALWAYS_ON_TOP).toBool());
  m_ui->autoHideCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_AUTOHIDE).toBool());
  m_ui->autoHideSlider->setValue(core->setting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY).toInt());
  m_ui->transparencyCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY).toBool());
  m_ui->transparencySlider->setValue(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toInt());
  m_ui->dockToEdgesCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_DOCKING).toBool());
  m_ui->dockDistanceSpinBox->setValue(core->setting(Settings::S_MAINWINDOW_DOCKING_DISTANCE).toInt());
  m_ui->mainWindowCaptionEdit->setText(core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString());
  m_ui->chatWindowCaptionEdit->setText(core->setting(Settings::S_CHATWINDOW_CAPTION, "%nick% [%status%] \"%description%\"").toString());
  m_ui->chatTabCaptionEdit->setText(core->setting(Settings::S_CHATTAB_CAPTION, "%name%").toString());
}
