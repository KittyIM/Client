#include "Core.h"

#include "widgets/windows/ProfilesWindow.h"
#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/ContactWindow.h"
#include "widgets/windows/HistoryWindow.h"
#include "widgets/windows/DebugWindow.h"
#include "widgets/windows/AboutWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/windows/MainWindow.h"
#include "../lib/hunspell/hunspell.hxx"
#include "EmoticonManager.h"
#include "ProtocolManager.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "PluginManager.h"
#include "ActionManager.h"
#include "JsonSettings.h"
#include "MessageQueue.h"
#include "IconManager.h"
#include "ChatManager.h"
#include "IconManager.h"
#include "constants.h"
#include "Profile.h"

#include <SDKConstants.h>
#include <IMessage.h>
#include <IChat.h>

#ifdef Q_WS_WIN32
#include <windows.h>
#include <winuser.h>
#endif

#include <QtCore/QCryptographicHash>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#define qDebug() qDebug() << "[Core]"
#define qWarning() qWarning() << "[Core]"

namespace Kitty
{

Core::Core()
{
	m_hunspell = 0;

	m_restart = false;
	m_portable = false;
}

Core::~Core()
{
	delete m_mainWindow;
	delete m_settingsWindow;
	delete m_chatWindow;

	delete m_hunspell;

	delete m_profilesWindow;
	delete m_historyWindow;
	delete m_actionManager;
	delete m_iconManager;

	delete m_profile;

	delete m_chatManager;
	delete m_contactManager;
	delete m_accountManager;

	ProtocolManager::destr();
	delete m_pluginManager;

	DebugWindow::destr();
}

QString Core::statusToString(const int &status)
{
	switch(status) {
		case KittySDK::IProtocol::Online:
			return tr("Online");
		break;

		case KittySDK::IProtocol::Away:
			return tr("Away");
		break;

		case KittySDK::IProtocol::FFC:
			return tr("Free for chat");
		break;

		case KittySDK::IProtocol::DND:
			return tr("Do not disturb");
		break;

		case KittySDK::IProtocol::Invisible:
			return tr("Invisible");
		break;

		case KittySDK::IProtocol::Offline:
			return tr("Offline");
		break;
	}

	return tr("Unknown");
}

QString Core::processUrls(const QString &text)
{
	QString result = text;

	const QRegExp urls("\\b((?:(?:https?|ftp|file)://|www\\.|ftp\\.))([-A-Z0-9+&@#/%=~_|$?!:,.]*[A-Z0-9+&@#/%=~_|$])", Qt::CaseInsensitive);
	int pos = 0;
	while((pos = urls.indexIn(result, pos)) != -1) {
		QString url = urls.cap(0);

		if(urls.cap(1) == "www.") {
			url.prepend("http://");
		} else if(urls.cap(1) == "ftp.") {
			url.prepend("ftp://");
		}

		QString link = QString("<a href=\"%1\">%2</a>").arg(url).arg(urls.cap(0));
		result.replace(pos, urls.cap(0).length(), link);
		pos += link.size();
	}

	const QRegExp emails("[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?", Qt::CaseInsensitive);
	pos = 0;
	while((pos = emails.indexIn(result, pos)) != -1) {
		QString mail = emails.cap(0);
		QString link = QString("<a href=\"mailto:%1\">%1</a>").arg(mail);
		result.replace(pos, mail.length(), link);
		pos += link.size();
	}

	return result;
}

QString Core::processEmoticons(const QString &text)
{
	return text;
}

QAction *Core::action(const QString &id)
{
	return actionManager()->action(id);
}

QPixmap Core::icon(const QString &id)
{
	return iconManager()->icon(id);
}

QVariant Core::setting(const QString &key, const QVariant &defaultValue)
{
	return settings()->value(key, defaultValue);
}

void Core::setSetting(const QString &key, const QVariant &value)
{
	return settings()->setValue(key, value);
}

void Core::loadProfile(const QString &name)
{
	profile()->load(name);

	mainWindow();
}

MainWindow *Core::mainWindow()
{
	if(!m_mainWindow) {
		m_mainWindow = new MainWindow(this);
	}

	return m_mainWindow;
}

ChatWindow *Core::chatWindow()
{
	if(!m_chatWindow) {
		m_chatWindow = new ChatWindow(this);
	}

	return m_chatWindow;
}

AboutWindow *Core::aboutWindow()
{
	if(!m_aboutWindow) {
		m_aboutWindow = new AboutWindow(this);
	}

	return m_aboutWindow;
}

ProfilesWindow *Core::profilesWindow()
{
	if(!m_profilesWindow) {
		m_profilesWindow = new ProfilesWindow(this);
	}

	return m_profilesWindow;
}

SettingsWindow *Core::settingsWindow()
{
	if(!m_settingsWindow) {
		m_settingsWindow = new SettingsWindow(this);
	}

	return m_settingsWindow;
}

HistoryWindow *Core::historyWindow()
{
	if(!m_historyWindow) {
		m_historyWindow = new HistoryWindow(this);
	}

	return m_historyWindow;
}

void Core::showMainWindow()
{
	mainWindow()->show();
}

void Core::showChatWindow()
{
	chatWindow()->show();
}

void Core::showProfilesWindow()
{
	profilesWindow()->show();
}

void Core::showSettingsWindow()
{
	settingsWindow()->show();
	settingsWindow()->activateWindow();
}

void Core::showHistoryWindow()
{
	historyWindow()->show();
}

void Core::showAddContactWindow()
{
	ContactWindow *wnd = new ContactWindow(this);
	wnd->show();
}

Profile *Core::profile()
{
	if(!m_profile) {
		m_profile = new Profile(this);
	}

	return m_profile;
}

Hunspell *Core::hunspell()
{
	if(!m_hunspell) {
		QByteArray dic = QString(qApp->applicationDirPath() + "/data/dictionaries/" + setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString() + ".dic").toLocal8Bit();
		QByteArray aff = QString(qApp->applicationDirPath() + "/data/dictionaries/" + setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString() + ".aff").toLocal8Bit();

		m_hunspell = new Hunspell(aff.constData(), dic.constData());

		//load custom words
		QFile customDict(currentProfileDir() + "custom.dic");
		if(customDict.open(QFile::ReadOnly | QFile::Text)) {
			while(!customDict.atEnd()) {
				m_hunspell->add(customDict.readLine().trimmed().constData());
			}

			customDict.close();
		}
	}

	return m_hunspell;
}

JsonSettings *Core::settings()
{
	return profile()->settings();
}

AccountManager *Core::accountManager()
{
	if(!m_accountManager) {
		m_accountManager = new AccountManager(this);
	}

	return m_accountManager;
}

ChatManager *Core::chatManager()
{
	if(!m_chatManager) {
		m_chatManager = new ChatManager(this);
	}

	return m_chatManager;
}

ActionManager *Core::actionManager()
{
	if(!m_actionManager) {
		m_actionManager = new ActionManager(this);
	}

	return m_actionManager;
}

ContactManager *Core::contactManager()
{
	if(!m_contactManager) {
		m_contactManager = new ContactManager(this);
	}

	return m_contactManager;
}

EmoticonManager *Core::emoticonManager()
{
	if(!m_emoticonManager) {
		m_emoticonManager = new EmoticonManager(this);
	}

	return m_emoticonManager;
}

IconManager *Core::iconManager()
{
	if(!m_iconManager) {
		m_iconManager = new IconManager(this);
	}

	return m_iconManager;
}

MessageQueue *Core::messageQueue()
{
	if(!m_messageQueue) {
		m_messageQueue = new MessageQueue(this);
	}

	return m_messageQueue;
}

PluginManager *Core::pluginManager()
{
	if(!m_pluginManager) {
		m_pluginManager = new PluginManager(this);
	}

	return m_pluginManager;
}

QString Core::avatarPath(KittySDK::IContact *contact) const
{
	if(contact) {
		KittySDK::IProtocol *proto = contact->protocol();
		if(proto) {
			return currentProfileDir() + "avatars/" + QCryptographicHash::hash(QString("avatar_" + proto->protoInfo()->protoName() + "_" + contact->uid()).toAscii(), QCryptographicHash::Md5).toHex() + ".png";
		}
	}

	return "";
}

QString Core::profileName() const
{
	return m_profile->name();
}

QString Core::profilesDir() const
{
	if(isPortable()) {
		return qApp->applicationDirPath() + "/profiles/";
	} else {
		return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/profiles/";
	}
}

QString Core::currentProfileDir() const
{
	return profilesDir() + profileName() + "/";
}

void Core::showAboutWindow()
{
	aboutWindow()->show();
}

void Core::restart()
{
	setRestart(true);
	qApp->quit();
}

void Core::changeProfile(const QString &profile, const QString &password)
{
	QStringList args = appArguments();
	qDebug() << args;

	//check if there's already profile info in args
	int i = args.indexOf("-profile");
	if(i > -1) {
		args.replace(i + 1, profile);
	} else {
		args.append("-profile");
		args.append(profile);
	}


	if(!password.isEmpty()) {
		args.append("-password");
		args.append(password);
	}

	setAppArguments(args);

	restart();
}

void Core::toggleMainWindow()
{
	if(mainWindow()->isVisible()) {
		if(mainWindow()->isObscured()) {
			mainWindow()->activateWindow();
			mainWindow()->raise();
		} else {
			mainWindow()->hide();
		}
	} else {
		mainWindow()->show();
		mainWindow()->activateWindow();
		mainWindow()->raise();
	}
}

void Core::openKittyFolder()
{
	QDesktopServices::openUrl(QUrl(qApp->applicationDirPath()));
}

void Core::openProfilesFolder()
{
	QDesktopServices::openUrl(QUrl(profilesDir()));
}

void Core::showContactWindow(KittySDK::IContact *cnt)
{
	KittySDK::IProtocol *proto = cnt->protocol();
	if(proto && proto->protoInfo()) {
		ContactWindow *wnd = m_contactWindows.value(proto->protoInfo()->protoName() + cnt->uid());
		if(!wnd) {
			wnd = new ContactWindow(this, cnt);
			m_contactWindows.insert(proto->protoInfo()->protoName() + cnt->uid(), wnd);
		}

		wnd->show();
		wnd->raise();
	}
}

// function taken from http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
bool Core::removeDir(const QString &dirName)
{
	bool result = true;
	QDir dir(dirName);

	if(dir.exists(dirName)) {
		foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if(info.isDir()) {
				result = removeDir(info.absoluteFilePath());
			} else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if(!result) {
				return result;
			}
		}

		result = dir.rmdir(dirName);
	}

	return result;
}

// TODO: add to PluginCore
void Core::archiveMessage(const KittySDK::IMessage &msg)
{
	if(setting(KittySDK::Settings::S_HISTORY_ENABLED, true).toBool()) {
		if(msg.to().count() > 1) {
			return;
		}

		QString protocol = msg.chat()->protocol()->protoInfo()->protoName();
		QString account = msg.chat()->account()->uid();
		QString fileName;

		if(msg.direction() == KittySDK::IMessage::Outgoing) {
			fileName = msg.singleTo()->uid();
		} else {
			fileName = msg.from()->uid();
		}

		//remove chars that are illegal in path names
		const QRegExp illegals("[/\\:*?\"<>|]");
		protocol.replace(illegals, "-");
		account.replace(illegals, "-");
		fileName.replace(illegals, "-");

		fileName += ".db";

		//let's be sure the path exists
		QString path = QString("history/%1/%2/").arg(protocol).arg(account);
		QDir dir(currentProfileDir() + path);
		if(!dir.exists()) {
			if(!dir.mkpath(".")) {
				qDebug() << "Can't create history path" << dir.absolutePath();
				return;
			}
		}

		QSqlDatabase db = QSqlDatabase::database();
		if(!db.isValid()) {
			db = QSqlDatabase::addDatabase("QSQLITE");
		}

		db.setDatabaseName(dir.absoluteFilePath(fileName));
		if(!db.open()) {
			qDebug() << "Failed to open history db" << db.databaseName() << db.lastError().text();
			return;
		}

		//create tables if necessary
		if(!db.tables().count()) {
			db.exec("CREATE TABLE 'messages' ("
					" 'id' INTEGER PRIMARY KEY AUTOINCREMENT,"
					" 'chatId' VARCHAR(40),"
					" 'timeStamp' INTEGER(11),"
					" 'dir' TINYINT(1),"
					" 'body' TEXT);");
		}

		//speed up sqlite
		db.exec("PRAGMA synchronous=NORMAL;");
		db.exec("PRAGMA temp_store=MEMORY;");
		db.exec("PRAGMA count_changes=OFF;");
		db.exec("PRAGMA journal_mode=OFF;");

		QSqlQuery query;
		query.prepare("INSERT INTO 'messages' ([chatID], [timeStamp], [dir], [body]) VALUES(:chatId, :timeStamp, :dir, :body);");
		query.bindValue(":chatId", msg.chat()->id());
		query.bindValue(":timeStamp", msg.timeStamp().toTime_t());
		query.bindValue(":dir", msg.direction());
		query.bindValue(":body", msg.body());

		if(!query.exec()) {
			qDebug() << "History insert failed" << query.lastError().text();
			db.close();

			return;
		}

		query.clear();
		db.close();
	}
}
}
