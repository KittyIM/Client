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
#include "ProtocolManager.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "PluginManager.h"
#include "ActionManager.h"
#include "SDK/constants.h"
#include "SDK/Message.h"
#include "JsonSettings.h"
#include "ChatManager.h"
#include "IconManager.h"
#include "constants.h"
#include "SDK/Chat.h"
#include "Profile.h"

#ifdef Q_WS_WIN32
#include <windows.h>
#include <winuser.h>
#endif

#include <QtCore/QCryptographicHash>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#define qDebug() qDebug() << "[Core]"
#define qWarning() qWarning() << "[Core]"

using namespace KittySDK;

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
	delete m_trayIcon;

	ActionManager::destr();
	IconManager::destr();

	delete m_profile;

	ChatManager::destr();
	ContactManager::destr();
	AccountManager::destr();

	ProtocolManager::destr();
	PluginManager::destr();

	DebugWindow::destroy();
}

QString Core::statusToString(const int &status)
{
	switch(status) {
		case Protocol::Online:
		return tr("Online");
		break;

		case Protocol::Away:
		return tr("Away");
		break;

		case Protocol::FFC:
		return tr("Free for chat");
		break;

		case Protocol::DND:
		return tr("Do not disturb");
		break;

		case Protocol::Invisible:
		return tr("Invisible");
		break;

		case Protocol::Offline:
		return tr("Offline");
		break;
	}

	return tr("Unknown");
}

QString Core::processUrls(const QString &text)
{
	QString result = text;

	QRegExp urls("\\b((?:(?:https?|ftp|file)://|www\\.|ftp\\.))([-A-Z0-9+&@#/%=~_|$?!:,.]*[A-Z0-9+&@#/%=~_|$])", Qt::CaseInsensitive);
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

	QRegExp emails("[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?", Qt::CaseInsensitive);
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

QAction *Core::action(const QString &id) const
{
	return ActionManager::inst()->action(id);
}

QPixmap Core::icon(const QString &id) const
{
	return IconManager::inst()->icon(id);
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

	DebugWindow::inst()->restoreGeometry(setting(Settings::S_DEBUGWINDOW_GEOMETRY).toByteArray());

	mainWindow();
}

MainWindow *Core::mainWindow()
{
	if(!m_mainWindow) {
		m_mainWindow = new MainWindow();
	}

	return m_mainWindow;
}

ChatWindow *Core::chatWindow()
{
	if(!m_chatWindow) {
		m_chatWindow = new ChatWindow();
	}

	return m_chatWindow;
}

AboutWindow *Core::aboutWindow()
{
	if(!m_aboutWindow) {
		m_aboutWindow = new AboutWindow();
	}

	return m_aboutWindow;
}

ProfilesWindow *Core::profilesWindow()
{
	if(!m_profilesWindow) {
		m_profilesWindow = new ProfilesWindow();
	}

	return m_profilesWindow;
}

SettingsWindow *Core::settingsWindow()
{
	if(!m_settingsWindow) {
		m_settingsWindow = new SettingsWindow();
	}

	return m_settingsWindow;
}

HistoryWindow *Core::historyWindow()
{
	if(!m_historyWindow) {
		m_historyWindow = new HistoryWindow();
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
	ContactWindow *wnd = new ContactWindow();
	wnd->show();
}

QSystemTrayIcon *Core::trayIcon()
{
	if(!m_trayIcon) {
		m_trayIcon = new QSystemTrayIcon(icon(Icons::I_KITTY));
		m_trayIcon->setToolTip(QString("KittyIM v%1").arg(Constants::VERSION));
		connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

		QMenu *menu = new QMenu(m_mainWindow);
		menu->addAction(action(Actions::A_SHOW_HIDE));
		menu->addSeparator();
		menu->addAction(action(Actions::A_SETTINGS));
		menu->addAction(action(Actions::A_QUIT));
		m_trayIcon->setContextMenu(menu);
	}

	return m_trayIcon;
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
		QByteArray dic = QString(qApp->applicationDirPath() + "/data/dictionaries/" + setting(Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString() + ".dic").toLocal8Bit();
		QByteArray aff = QString(qApp->applicationDirPath() + "/data/dictionaries/" + setting(Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString() + ".aff").toLocal8Bit();

		m_hunspell = new Hunspell(aff.constData(), dic.constData());
	}

	return m_hunspell;
}

JsonSettings *Core::settings()
{
	return profile()->settings();
}

QString Core::avatarPath(KittySDK::Contact *contact) const
{
	if(contact) {
		Protocol *proto = contact->protocol();
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

void Core::showTrayIcon()
{
	trayIcon()->show();
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

void Core::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger) {
		action(Actions::A_SHOW_HIDE)->trigger();
	}
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

void Core::showContactWindow(KittySDK::Contact *cnt)
{
	Protocol *proto = cnt->protocol();
	if(proto && proto->protoInfo()) {
		ContactWindow *wnd = m_contactWindows.value(proto->protoInfo()->protoName() + cnt->uid());
		if(!wnd) {
			wnd = new ContactWindow(cnt);
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
bool Core::archiveMessage(const KittySDK::Message &msg)
{
	if(setting(Settings::S_HISTORY_ENABLED, true).toBool()) {
		QString protocol = msg.chat()->protocol()->protoInfo()->protoName();
		QString account = msg.chat()->account()->uid();
		QString fileName;

		if(msg.direction() == Message::Outgoing) {
			fileName = msg.singleTo()->uid();
		} else {
			fileName = msg.from()->uid();
		}

		//remove chars that are illegal in path names
		protocol.replace(QRegExp("[/\\:*?\"<>|]"), "-");
		account.replace(QRegExp("[/\\:*?\"<>|]"), "-");
		fileName.replace(QRegExp("[/\\:*?\"<>|]"), "-");

		fileName += ".db";

		//let's be sure the path exists
		QString path = QString("history/%1/%2/").arg(protocol).arg(account);
		QDir dir(currentProfileDir() + path);
		if(!dir.exists()) {
			if(!dir.mkpath(".")) {
				qDebug() << "Can't create history path" << dir.absolutePath();
				return false;
			}
		}

		QSqlDatabase db = QSqlDatabase::database();
		if(!db.isValid()) {
			db = QSqlDatabase::addDatabase("QSQLITE");
		}

		db.setDatabaseName(dir.absoluteFilePath(fileName));
		if(!db.open()) {
			qDebug() << "Failed to open history db" << db.databaseName() << db.lastError().text();
			return false;
		}

		//create tables if necessary
		if(!db.tables().count()) {
			db.exec("CREATE TABLE 'messages' ("
					" 'id' INTEGER PRIMARY KEY AUTOINCREMENT,"
					" 'chatId' INTEGER,"
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
			qDebug() << "Insert failed" << query.lastError().text();
			db.close();

			return false;
		}

		query.clear();
		db.close();
	}

	return true;
}
}
