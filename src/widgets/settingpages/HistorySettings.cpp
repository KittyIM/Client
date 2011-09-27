#include "HistorySettings.h"
#include "ui_HistorySettings.h"

#include "SDK/constants.h"
#include "Core.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::HistorySettings::HistorySettings(QWidget *parent): SettingPage(0, parent), m_ui(new Ui::HistorySettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_HISTORY);
}

Kitty::HistorySettings::~HistorySettings()
{
  delete m_ui;
}

void Kitty::HistorySettings::apply()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_HISTORY_ENABLED, m_ui->historyCheckBox->isChecked());
  core->setSetting(Settings::S_HISTORY_STRANGERS, m_ui->saveStrangerHistoryCheckBox->isChecked());
  core->setSetting(Settings::S_HISTORY_EXPIRE_DAYS, m_ui->expireSpinBox->value());
  core->setSetting(Settings::S_HISTORY_UNDERLINE_LINKS, m_ui->underlineLinksCheckBox->isChecked());
  core->setSetting(Settings::S_HISTORY_FORMATTING, m_ui->displayFormattingCheckBox->isChecked());
}

void Kitty::HistorySettings::reset()
{
  Core *core = Core::inst();

  m_ui->historyCheckBox->setChecked(core->setting(Settings::S_HISTORY_ENABLED, true).toBool());
  m_ui->saveStrangerHistoryCheckBox->setChecked(core->setting(Settings::S_HISTORY_STRANGERS, true).toBool());
  m_ui->expireSpinBox->setValue(core->setting(Settings::S_HISTORY_EXPIRE_DAYS, 0).toInt());
  m_ui->underlineLinksCheckBox->setChecked(core->setting(Settings::S_HISTORY_UNDERLINE_LINKS, true).toBool());
  m_ui->displayFormattingCheckBox->setChecked(core->setting(Settings::S_HISTORY_FORMATTING, true).toBool());

}

void Kitty::HistorySettings::retranslate()
{
  m_ui->retranslateUi(this);
}
