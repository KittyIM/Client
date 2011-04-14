#include "DisplaySettings.h"
#include "ui_DisplaySettings.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtGui/QToolTip>

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
  Kitty::Core *core = Kitty::Core::inst();

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
  Kitty::Core *core = Kitty::Core::inst();

  m_ui->alwaysOnTopCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_ALWAYS_ON_TOP).toBool());
  m_ui->autoHideCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_AUTOHIDE).toBool());
  m_ui->autoHideSlider->setValue(core->setting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY, 5).toInt());
  m_ui->transparencyCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY).toBool());
  m_ui->transparencySlider->setValue(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toInt());
  m_ui->dockToEdgesCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_DOCKING).toBool());
  m_ui->dockDistanceSpinBox->setValue(core->setting(Settings::S_MAINWINDOW_DOCKING_DISTANCE).toInt());
  m_ui->mainWindowCaptionEdit->setText(core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString());
  m_ui->chatWindowCaptionEdit->setText(core->setting(Settings::S_CHATWINDOW_CAPTION, "%nick% [%status%] \"%description%\"").toString());
  m_ui->chatTabCaptionEdit->setText(core->setting(Settings::S_CHATTAB_CAPTION, "%name%").toString());
}

void Kitty::DisplaySettings::updateIcons()
{
  m_ui->mainWindowCaptionHelpButton->setIcon(Kitty::Core::inst()->icon(Icons::I_INFO));
  m_ui->chatWindowCaptionHelpButton->setIcon(Kitty::Core::inst()->icon(Icons::I_INFO));
  m_ui->chatTabCaptionHelpButton->setIcon(Kitty::Core::inst()->icon(Icons::I_INFO));
}

void Kitty::DisplaySettings::on_mainWindowCaptionHelpButton_clicked()
{
  QToolTip::showText(m_ui->mainWindowCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
                                                                                   "<br><i>%version%</i> - " + tr("Kitty's version") +
                                                                                   "<br><i>%profile%</i> - " + tr("name of loaded profile"));
}

void Kitty::DisplaySettings::on_chatWindowCaptionHelpButton_clicked()
{
  QToolTip::showText(m_ui->chatWindowCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
                                                                                   "<br><i>%nick%</i> - " + tr("user's nickname") +
                                                                                   "<br><i>%status%</i> - " + tr("user's status") +
                                                                                   "<br><i>%description%</i> - " + tr("user's status description") +
                                                                                   "<br><i>%uid%</i> -" + tr("user's id") +
                                                                                   "<br><i>%gender%</i> -" + tr("user's gender") +
                                                                                   "<br><i>%birthday%</i> -" + tr("user's birthday") +
                                                                                   "<br><i>%phone%</i> -" + tr("user's phone") +
                                                                                   "<br><i>%email%</i> -" + tr("user's email") +
                                                                                   "<br><i>%city%</i> -" + tr("user's city"));
}

void Kitty::DisplaySettings::on_chatTabCaptionHelpButton_clicked()
{
  QToolTip::showText(m_ui->chatTabCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
                                                                                   "<br><i>%nick%</i> - " + tr("user's nickname") +
                                                                                   "<br><i>%status%</i> - " + tr("user's status") +
                                                                                   "<br><i>%description%</i> - " + tr("user's status description") +
                                                                                   "<br><i>%unread%</i> -" + tr("unread messages"));

}
