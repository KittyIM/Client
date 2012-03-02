#include "ChatWindowEditSettings.h"
#include "ui_ChatWindowEditSettings.h"

#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QFontDatabase>

namespace Kitty
{

ChatWindowEditSettings::ChatWindowEditSettings(QWidget *parent) :
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::ChatWindowEditSettings)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_BULLET);
	setId(KittySDK::SettingPages::S_DISPLAY_CHATWINDOW_EDIT);

	connect(m_ui->fontFamilyComboBox, SIGNAL(currentFontChanged(QFont)), SLOT(updateFontSizes()));
}

ChatWindowEditSettings::~ChatWindowEditSettings()
{
	delete m_ui;
}

void ChatWindowEditSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

void ChatWindowEditSettings::apply()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_ENABLED, m_ui->spellCheckCheckBox->isChecked());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_DICT, m_ui->spellCheckDictionaryComboBox->itemData(m_ui->spellCheckDictionaryComboBox->currentIndex()).toString());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_SUGGESTIONS, m_ui->spellCheckSuggestionsSpinBox->value());
	core->setSetting(KittySDK::Settings::S_CHATEDIT_FONT_FAMILY, m_ui->fontFamilyComboBox->currentFont().family());
	core->setSetting(KittySDK::Settings::S_CHATEDIT_FONT_SIZE, m_ui->fontSizeComboBox->currentText());
	core->setSetting(KittySDK::Settings::S_CHATEDIT_FONT_ANTIALIASING, m_ui->antialiasingCheckBox->isChecked());
}

void ChatWindowEditSettings::reset()
{
	Core *core = Core::inst();

	QFont font = qApp->font();
	font.setFamily(core->setting(KittySDK::Settings::S_CHATEDIT_FONT_FAMILY, font.family()).toString());
	m_ui->fontFamilyComboBox->setCurrentFont(font);

	updateFontSizes();

	m_ui->antialiasingCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATEDIT_FONT_ANTIALIASING, false).toBool());

	m_ui->spellCheckCheckBox->setChecked(core->setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_ENABLED, false).toBool());
	m_ui->spellCheckSuggestionsSpinBox->setValue(core->setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_SUGGESTIONS, 7).toInt());

	m_ui->spellCheckDictionaryComboBox->clear();
	m_ui->spellCheckDictionaryComboBox->addItem(tr("None"), QString());

	QDir dicDir(qApp->applicationDirPath() + "/data/dictionaries/");
	QFileInfoList dics = dicDir.entryInfoList(QStringList() << "*.dic");

	foreach(QFileInfo file, dics) {
		QString lang = file.baseName();
		if(dicDir.exists(lang + ".aff")) {
			m_ui->spellCheckDictionaryComboBox->addItem(QString("%1 (%2)").arg(QLocale::languageToString(QLocale(lang).language())).arg(lang), lang);

			if(lang == core->setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString()) {
				m_ui->spellCheckDictionaryComboBox->setCurrentIndex(m_ui->spellCheckDictionaryComboBox->count() - 1);
			}
		} else {
			qWarning() << "Dictionary file" << file.fileName() << "has no .aff companion";
		}
	}
}

void ChatWindowEditSettings::updateFontSizes()
{
	int current_size = Core::inst()->setting(KittySDK::Settings::S_CHATEDIT_FONT_SIZE, qApp->font().pointSize()).toInt();
	QFontDatabase db;

	m_ui->fontSizeComboBox->clear();

	foreach(const int &size, db.pointSizes(m_ui->fontFamilyComboBox->currentFont().family())) {
		m_ui->fontSizeComboBox->addItem(QString::number(size));

		if(size == current_size) {
			m_ui->fontSizeComboBox->setCurrentIndex( m_ui->fontSizeComboBox->count() - 1);
		}
	}
}

}
