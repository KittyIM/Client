#include "ChatWindowSettings.h"
#include "ui_ChatWindowSettings.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QDir>

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatWindowSettings::ChatWindowSettings(QWidget *parent): SettingPage(parent), m_ui(new Ui::ChatWindowSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_MESSAGE);
}

Kitty::ChatWindowSettings::~ChatWindowSettings()
{
  delete m_ui;
}

void Kitty::ChatWindowSettings::apply()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_CHATWINDOW_COPYSELECTION, m_ui->copySelectionToClipboardCheckBox->isChecked());
  core->setSetting(Settings::S_CHATWINDOW_SENTHISTORY, m_ui->sentMessagesHistoryCheckBox->isChecked());
  core->setSetting(Settings::S_CHATWINDOW_SPELLCHECK_ENABLED, m_ui->spellCheckCheckBox->isChecked());
  core->setSetting(Settings::S_CHATWINDOW_SPELLCHECK_DICT, m_ui->spellCheckDictionaryComboBox->itemData(m_ui->spellCheckDictionaryComboBox->currentIndex()).toString());
}

void Kitty::ChatWindowSettings::reset()
{
  Core *core = Core::inst();

  m_ui->copySelectionToClipboardCheckBox->setChecked(core->setting(Settings::S_CHATWINDOW_COPYSELECTION, false).toBool());
  m_ui->sentMessagesHistoryCheckBox->setChecked(core->setting(Settings::S_CHATWINDOW_SENTHISTORY, true).toBool());
  m_ui->spellCheckCheckBox->setChecked(core->setting(Settings::S_CHATWINDOW_SPELLCHECK_ENABLED, false).toBool());

  m_ui->spellCheckDictionaryComboBox->clear();
  m_ui->spellCheckDictionaryComboBox->addItem(tr("None"), QString());

  QDir dicDir(qApp->applicationDirPath() + "/data/dictionaries/");
  QFileInfoList dics = dicDir.entryInfoList(QStringList() << "*.dic");
  foreach(QFileInfo file, dics) {
    QString lang = file.baseName();
    if(dicDir.exists(lang + ".aff")) {
      m_ui->spellCheckDictionaryComboBox->addItem(QString("%1 (%2)").arg(QLocale::languageToString(QLocale(lang).language())).arg(lang), lang);

      if(lang == core->setting(Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString()) {
        m_ui->spellCheckDictionaryComboBox->setCurrentIndex(m_ui->spellCheckDictionaryComboBox->count() - 1);
      }
    } else {
      qWarning() << "Dictionary file" << file.fileName() << "has no .aff companion";
    }
  }
}
