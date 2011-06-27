#include "DisplaySettings.h"
#include "ui_DisplaySettings.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtGui/QToolTip>

using namespace Kitty;
using namespace KittySDK;

Kitty::DisplaySettings::DisplaySettings(QWidget *parent): KittySDK::SettingPage(0, parent), m_ui(new Ui::DisplaySettings)
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
  m_ui->autoHideSlider->setValue(core->setting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY, 5).toInt());
  m_ui->transparencyCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY).toBool());
  m_ui->transparencySlider->setValue(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toInt());
  m_ui->dockToEdgesCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_DOCKING).toBool());
  m_ui->dockDistanceSpinBox->setValue(core->setting(Settings::S_MAINWINDOW_DOCKING_DISTANCE).toInt());
  m_ui->mainWindowCaptionEdit->setText(core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString());
  m_ui->chatWindowCaptionEdit->setText(core->setting(Settings::S_CHATWINDOW_CAPTION, "%display% [%status%] %description%").toString());
  m_ui->chatTabCaptionEdit->setText(core->setting(Settings::S_CHATTAB_CAPTION, "%display%").toString());
}

void Kitty::DisplaySettings::updateIcons()
{
  m_ui->mainWindowCaptionHelpButton->setIcon(Core::inst()->icon(Icons::I_INFO));
  m_ui->chatWindowCaptionHelpButton->setIcon(Core::inst()->icon(Icons::I_INFO));
  m_ui->chatTabCaptionHelpButton->setIcon(Core::inst()->icon(Icons::I_INFO));
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
                                                                                   "<br><i>%display%</i> - " + tr("contact's name") +
                                                                                   "<br><i>%nickname%</i> - " + tr("contact's nickname") +
                                                                                   "<br><i>%firstname%</i> - " + tr("contact's first name") +
                                                                                   "<br><i>%lastname%</i> - " + tr("contact's last name") +
                                                                                   "<br><i>%status%</i> - " + tr("contact's status") +
                                                                                   "<br><i>%description%</i> - " + tr("contact's status description") +
                                                                                   "<br><i>%uid%</i> -" + tr("contact's id") +
                                                                                   "<br><i>%sex%</i> -" + tr("contact's sex") +
                                                                                   "<br><i>%birthday%</i> -" + tr("contact's birthday") +
                                                                                   "<br><i>%phone%</i> -" + tr("contact's phone") +
                                                                                   "<br><i>%email%</i> -" + tr("contact's email"));
}

void Kitty::DisplaySettings::on_chatTabCaptionHelpButton_clicked()
{
  QToolTip::showText(m_ui->chatTabCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
                                                                                "<br><i>%display%</i> - " + tr("contact's name") +
                                                                                "<br><i>%nickname%</i> - " + tr("contact's nickname") +
                                                                                "<br><i>%firstname%</i> - " + tr("contact's first name") +
                                                                                "<br><i>%lastname%</i> - " + tr("contact's last name") +
                                                                                "<br><i>%status%</i> - " + tr("contact's status") +
                                                                                "<br><i>%description%</i> - " + tr("contact's status description") +
                                                                                "<br><i>%uid%</i> -" + tr("contact's id") +
                                                                                "<br><i>%sex%</i> -" + tr("contact's sex") +
                                                                                "<br><i>%birthday%</i> -" + tr("contact's birthday") +
                                                                                "<br><i>%phone%</i> -" + tr("contact's phone") +
                                                                                "<br><i>%email%</i> -" + tr("contact's email"));

}
