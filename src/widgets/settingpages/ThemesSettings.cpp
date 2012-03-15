#include "ThemesSettings.h"
#include "ui_ThemesSettings.h"

#include "PluginCoreImpl.h"
#include "IconTheme.h"
#include "ChatTheme.h"
#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IProtocol.h>
#include <IAccount.h>
#include <IContact.h>
#include <IMessage.h>

#include <QtCore/QFileInfoList>
#include <QtCore/QDateTime>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>

namespace Kitty
{

class KittyAccount: public KittySDK::IAccount
{
	public:
		KittyAccount(const QString &uid, KittySDK::IProtocol *parent): KittySDK::IAccount(uid, parent)
		{
			setMe(new KittySDK::IContact("me@example.com", this));
			me()->setDisplay(protocol()->core()->profileName());
		}

		~KittyAccount()
		{
			delete m_me;
		}

		KittySDK::IContact *newContact(const QString &uid){ return new KittySDK::IContact(uid, this); }
};

class KittyProto: public KittySDK::IProtocol
{
	public:
		KittyProto(KittySDK::IPluginCore *core): KittySDK::IProtocol(core)
		{
			m_info = new KittySDK::IProtocolInfo();
			static_cast<KittySDK::IProtocolInfo*>(m_info)->setProtoName("KittyIM");
		}

		~KittyProto()
		{
			delete m_info;
		}

		KittySDK::IAccount *newAccount(const QString &uid) { return new KittyAccount(uid, this); }
		QDialog *editDialog(KittySDK::IAccount *) { return 0; }
};

ThemesSettings::ThemesSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::ThemesSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	setIcon(KittySDK::Icons::I_BULLET);
	setId(KittySDK::SettingPages::S_DISPLAY_THEMES);

	connect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateVariantList()));
	connect(m_ui->iconThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIconsPreview()));
}

ThemesSettings::~ThemesSettings()
{
	delete m_ui;
}

void ThemesSettings::apply()
{
	m_core->setSetting(KittySDK::Settings::S_ROSTER_THEME, m_ui->rosterThemeComboBox->itemData(m_ui->rosterThemeComboBox->currentIndex()));
	m_core->setSetting(KittySDK::Settings::S_CHAT_THEME, m_ui->chatThemeComboBox->itemData(m_ui->chatThemeComboBox->currentIndex()));
	m_core->setSetting(KittySDK::Settings::S_CHAT_THEME_VARIANT, m_ui->chatThemeVariantComboBox->itemData(m_ui->chatThemeVariantComboBox->currentIndex()));
	m_core->setSetting(KittySDK::Settings::S_ICON_THEME, m_ui->iconThemeComboBox->itemData(m_ui->iconThemeComboBox->currentIndex()));
}

void ThemesSettings::reset()
{
	//chat themes
	disconnect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));
	disconnect(m_ui->chatThemeVariantComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));

	m_ui->chatThemeComboBox->clear();
	m_ui->chatThemeComboBox->addItem(tr("Default"), QString());

	QDir chatThemeDir(qApp->applicationDirPath() + "/themes/chat/");
	QFileInfoList chatThemes = chatThemeDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo dir, chatThemes) {
		m_ui->chatThemeComboBox->addItem(dir.fileName(), dir.fileName());

		if(dir.fileName() == m_core->setting(KittySDK::Settings::S_CHAT_THEME, QString()).toString()) {
			m_ui->chatThemeComboBox->setCurrentIndex(m_ui->chatThemeComboBox->count() - 1);
		}
	}

	updateVariantList();

	//roster themes
	m_ui->rosterThemeComboBox->clear();
	m_ui->rosterThemeComboBox->addItem(tr("Default"), QString());

	QDir rosterThemeDir(qApp->applicationDirPath() + "/themes/roster/");
	QFileInfoList rosterThemes = rosterThemeDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo dir, rosterThemes) {
		m_ui->rosterThemeComboBox->addItem(dir.fileName(), dir.fileName());

		if(dir.fileName() == m_core->setting(KittySDK::Settings::S_ROSTER_THEME, QString()).toString()) {
			m_ui->rosterThemeComboBox->setCurrentIndex(m_ui->rosterThemeComboBox->count() - 1);
		}
	}

	//icon themes
	m_ui->iconThemeComboBox->clear();
	m_ui->iconThemeComboBox->addItem(tr("Default"), QString());
	QDir iconThemeDir(qApp->applicationDirPath() + "/themes/icon/");
	QFileInfoList iconThemes = iconThemeDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo dir, iconThemes) {
		IconTheme theme(dir.fileName());

		QString name = dir.fileName();
		if(!theme.author().isEmpty()) {
			name.append(QString(" %1 %2").arg(tr("by")).arg(theme.author()));
		}

		if(!theme.email().isEmpty()) {
			name.append(QString("<%1>").arg(theme.email()));
		}

		m_ui->iconThemeComboBox->addItem(name, dir.fileName());

		if(dir.fileName() == m_core->setting(KittySDK::Settings::S_ICON_THEME, QString()).toString()) {
			m_ui->iconThemeComboBox->setCurrentIndex(m_ui->iconThemeComboBox->count() - 1);
		}
	}
}

void ThemesSettings::updateChatPreview()
{
	m_ui->chatThemeWebView->clearTo(true, m_ui->chatThemeComboBox->itemData(m_ui->chatThemeComboBox->currentIndex()).toString(), m_ui->chatThemeVariantComboBox->itemData(m_ui->chatThemeVariantComboBox->currentIndex()).toString());

	ChatTheme theme(m_ui->chatThemeComboBox->currentText());

	PluginCoreImpl core;
	KittyProto proto(&core);
	if(KittyAccount *acc = dynamic_cast<KittyAccount*>(proto.newAccount("me@example.com"))) {
		if(KittySDK::IContact *kittyBot = acc->newContact("kittybot@kittyim.pl")) {
			kittyBot->setDisplay("KittyBot");

			KittySDK::IMessage outgoing(acc->me(), kittyBot);
			outgoing.setBody("Hello KittyBot!");
			m_ui->chatThemeWebView->appendMessage(outgoing, &theme);

			KittySDK::IMessage incoming(kittyBot, acc->me());
			incoming.setDirection(KittySDK::IMessage::Incoming);
			incoming.setBody("Oh hi, I didn't notice you there ;)");
			m_ui->chatThemeWebView->appendMessage(incoming, &theme);

			incoming.setTimeStamp(QDateTime::currentDateTime());
			incoming.setBody("Could you please change your status to online?");
			m_ui->chatThemeWebView->appendMessage(incoming, &theme);

			KittySDK::IMessage status(acc->me(), acc->me());
			status.setBody(QString("%1 changed status to online").arg(acc->me()->display()));
			status.setDirection(KittySDK::IMessage::System);
			m_ui->chatThemeWebView->appendMessage(status, &theme);

			outgoing.setTimeStamp(QDateTime::currentDateTime());
			outgoing.setBody("How's that?");
			m_ui->chatThemeWebView->appendMessage(outgoing, &theme);

			incoming.setTimeStamp(QDateTime::currentDateTime());
			incoming.setBody("Awesome, thanks!");
			m_ui->chatThemeWebView->appendMessage(incoming, &theme);

			delete kittyBot;
		}

		delete acc;
	}
}

void ThemesSettings::updateVariantList()
{
	disconnect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));
	disconnect(m_ui->chatThemeVariantComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));

	m_ui->chatThemeVariantComboBox->clear();

	QString path = ":/chat";
	if(!m_ui->chatThemeComboBox->itemData(m_ui->chatThemeComboBox->currentIndex()).toString().isEmpty()) {
		path = qApp->applicationDirPath() + "/themes/chat/" + m_ui->chatThemeComboBox->currentText();
	}

	QDir variantDir(path + "/Variants");
	QFileInfoList variants = variantDir.entryInfoList(QStringList() << "*.css");
	foreach(QFileInfo var, variants) {
		m_ui->chatThemeVariantComboBox->addItem(var.completeBaseName(), var.fileName());

		if(var.fileName() == m_core->setting(KittySDK::Settings::S_CHAT_THEME_VARIANT, QString()).toString()) {
			m_ui->chatThemeVariantComboBox->setCurrentIndex(m_ui->chatThemeVariantComboBox->count() - 1);
		}
	}

	connect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));
	connect(m_ui->chatThemeVariantComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));

	updateChatPreview();
}

void ThemesSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

void ThemesSettings::updateIconsPreview()
{
	IconTheme theme(m_ui->iconThemeComboBox->itemData(m_ui->iconThemeComboBox->currentIndex()).toString());
	m_ui->iconThemeListWidget->clear();

	QHashIterator<QString, QString> it(theme.icons());
	while(it.hasNext()) {
		it.next();

		QListWidgetItem *item = new QListWidgetItem(m_ui->iconThemeListWidget);
		item->setIcon(QIcon(it.value()));
		item->setToolTip(it.key());
	}
}

}
