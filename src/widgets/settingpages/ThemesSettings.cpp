#include "ThemesSettings.h"
#include "ui_ThemesSettings.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QFileInfoList>
#include <QtCore/QDir>

using namespace KittySDK;

Kitty::ThemesSettings::ThemesSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::ThemesSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_BULLET);
}

Kitty::ThemesSettings::~ThemesSettings()
{
  delete m_ui;
}

void Kitty::ThemesSettings::apply()
{
  Kitty::Core *core = Kitty::Core::inst();
  core->setSetting(Settings::S_ROSTER_THEME, m_ui->rosterThemeComboBox->itemData(m_ui->rosterThemeComboBox->currentIndex()));
}

void Kitty::ThemesSettings::reset()
{
  Kitty::Core *core = Kitty::Core::inst();

  m_ui->rosterThemeComboBox->clear();
  m_ui->rosterThemeComboBox->addItem(tr("Default"), QString());

  QDir langDir(qApp->applicationDirPath() + "/themes/roster/");
  QFileInfoList langs = langDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  foreach(QFileInfo dir, langs) {
    m_ui->rosterThemeComboBox->addItem(dir.fileName(), dir.fileName());

    if(dir.fileName() == core->setting(Settings::S_ROSTER_THEME, QString()).toString()) {
      m_ui->rosterThemeComboBox->setCurrentIndex(m_ui->rosterThemeComboBox->count() - 1);
    }
  }
}
