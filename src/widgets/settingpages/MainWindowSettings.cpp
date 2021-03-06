#include "MainWindowSettings.h"
#include "ui_MainWindowSettings.h"

#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

MainWindowSettings::MainWindowSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::MainWindowSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_BULLET);
	setId(KittySDK::SettingPages::S_DISPLAY_MAINWINDOW);
}

MainWindowSettings::~MainWindowSettings()
{
	delete m_ui;
}

void MainWindowSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

void MainWindowSettings::apply()
{
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_ALWAYS_ON_TOP, m_ui->alwaysOnTopCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_AUTOHIDE, m_ui->autoHideCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_AUTOHIDE_DELAY, m_ui->autoHideSlider->value());
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY, m_ui->transparencyCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, m_ui->transparencySlider->value());
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_DOCKING, m_ui->dockToEdgesCheckBox->isChecked());
	m_core->setSetting(KittySDK::Settings::S_MAINWINDOW_DOCKING_DISTANCE, m_ui->dockDistanceSpinBox->value());
}

void MainWindowSettings::reset()
{
	m_ui->alwaysOnTopCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_MAINWINDOW_ALWAYS_ON_TOP).toBool());
	m_ui->autoHideCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_MAINWINDOW_AUTOHIDE).toBool());
	m_ui->autoHideSlider->setValue(m_core->setting(KittySDK::Settings::S_MAINWINDOW_AUTOHIDE_DELAY, 5).toInt());
	m_ui->transparencyCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY).toBool());
	m_ui->transparencySlider->setValue(m_core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toInt());
	m_ui->dockToEdgesCheckBox->setChecked(m_core->setting(KittySDK::Settings::S_MAINWINDOW_DOCKING).toBool());
	m_ui->dockDistanceSpinBox->setValue(m_core->setting(KittySDK::Settings::S_MAINWINDOW_DOCKING_DISTANCE).toInt());
}

}
