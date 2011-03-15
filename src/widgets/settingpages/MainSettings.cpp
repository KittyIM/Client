#include "MainSettings.h"
#include "ui_MainSettings.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QFileInfoList>
#include <QtCore/QDir>

using namespace KittySDK;

Kitty::MainSettings::MainSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::MainSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_SETTINGS);
}

Kitty::MainSettings::~MainSettings()
{
  delete m_ui;
}

void Kitty::MainSettings::apply()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_LANGUAGE, m_ui->languageComboBox->itemData(m_ui->languageComboBox->currentIndex()).toString());
}

void Kitty::MainSettings::reset()
{
  Core *core = Core::inst();

  m_ui->languageComboBox->clear();
  m_ui->languageComboBox->addItem(tr("System"), "");
  m_ui->languageComboBox->addItem("English", "C");

  if(core->setting(Settings::S_LANGUAGE, "C").toString() == "C") {
    m_ui->languageComboBox->setCurrentIndex(0);
  }

  QDir langDir(qApp->applicationDirPath() + "/data/translations/");
  QFileInfoList langs = langDir.entryInfoList(QStringList() << "kitty_*.qm");
  foreach(QFileInfo file, langs) {
    QString locale = file.baseName().replace("kitty_", "");
    m_ui->languageComboBox->addItem(QLocale::languageToString(QLocale(locale).language()), locale);

    if(locale == core->setting(Settings::S_LANGUAGE, "C").toString()) {
      m_ui->languageComboBox->setCurrentIndex(m_ui->languageComboBox->count() - 1);
    }
  }
}
