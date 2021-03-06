#include "MainSettings.h"
#include "ui_MainSettings.h"

#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QFileInfoList>
#include <QtCore/QDebug>
#include <QtCore/QDir>

namespace Kitty
{

MainSettings::MainSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::MainSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_SETTINGS);
	setId(KittySDK::SettingPages::S_SETTINGS);
}

MainSettings::~MainSettings()
{
	delete m_ui;
}

void MainSettings::retranslate()
{
	m_ui->retranslateUi(this);

	if(m_ui->languageComboBox->count()) {
		m_ui->languageComboBox->setItemText(0, tr("System"));
	}
}

void MainSettings::apply()
{
	m_core->setSetting(KittySDK::Settings::S_LANGUAGE, m_ui->languageComboBox->itemData(m_ui->languageComboBox->currentIndex()).toString());
}

void MainSettings::reset()
{
	m_ui->languageComboBox->clear();
	m_ui->languageComboBox->addItem(tr("System"), QString());
	m_ui->languageComboBox->addItem("English", "C");

	if(m_core->setting(KittySDK::Settings::S_LANGUAGE, "C").toString() == "C") {
		m_ui->languageComboBox->setCurrentIndex(1);
	}

	QDir langDir(qApp->applicationDirPath() + "/data/translations/");
	QFileInfoList langs = langDir.entryInfoList(QStringList() << "kitty_*.qm");
	foreach(QFileInfo file, langs) {
		QString locale = file.baseName().replace("kitty_", "");
		m_ui->languageComboBox->addItem(QLocale::languageToString(QLocale(locale).language()), locale);

		if(locale == m_core->setting(KittySDK::Settings::S_LANGUAGE, "C").toString()) {
			m_ui->languageComboBox->setCurrentIndex(m_ui->languageComboBox->count() - 1);
		}
	}
}

}
