#include "App.h"

#include "widgets/windows/DebugWindow.h"
#include "JsonSettings.h"
#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkProxy>
#include <QtWebKit/QWebSettings>

#define qDebug() qDebug() << "[App]"
#define qWarning() qWarning() << "[App]"

namespace Kitty
{

App::App(int &argc, char **argv):
	QApplication(argc, argv)
{
	m_startDate = QDateTime::currentDateTime();

	m_translator = 0;
	m_qtTranslator = 0;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	QCoreApplication::setApplicationName("KittyIM");
	QWebSettings::globalSettings()->setFontFamily(QWebSettings::StandardFont, font().family());

	qInstallMsgHandler(DebugWindow::addMessage);
	setQuitOnLastWindowClosed(false);

	connect(this, SIGNAL(aboutToQuit()), SLOT(cleanUp()));

	qDebug() << "Starting KittyIM";
	qDebug() << "There are" << (arguments().count() - 1) << "arguments";

	Core *core = Core::inst();

	QString profile;
	QString password;
	QListIterator<QString> it(arguments());
	while(it.hasNext()) {
		QString arg = it.next();
		if(arg == "-debug") {
			qDebug() << "-debug found, showing debug console";
			core->debugWindow()->show();
		} else if(arg == "-profile") {
			if(it.hasNext()) {
				profile = it.next();
				qDebug() << "-profile found, profile is" << profile;
			} else {
				qWarning() << "-profile found but nothing more";
			}
		} else if(arg == "-password") {
			if(it.hasNext()) {
				password = it.next();
				qDebug() << "-password found";
			} else {
				qWarning() << "-password found but nothing more";
			}
		} else if(arg == "-portable") {
			qDebug() << "-portable found, we are going portable";
			core->setPortable(true);
		}
	}

	core->setAppArguments(arguments());

	// let's check if portability file exists
	if(!core->isPortable()) {
		if(QFile::exists(applicationDirPath() + "/data/portable")) {
			core->setPortable(true);
		}
	}

	// there was no info in arguments, but maybe there's only 1 account
	if(profile.isEmpty()) {
		QDir dir(Core::inst()->profilesDir());
		QStringList profiles = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		if(profiles.count() == 1) {
			qDebug() << "Only one profile, let's try to load it!";
			profile = profiles[0];
		}
	}

	if(!profile.isEmpty()) {
		JsonSettings set(Core::inst()->profilesDir() + profile + "/settings.dat");
		bool hasPassword = !set.value(KittySDK::Settings::S_PROFILE_PASSWORD).toString().isEmpty();

		if(!hasPassword) {
			qDebug() << "Profile is ok, loading.";
			core->loadProfile(profile);
		} else {
			if(!password.isEmpty()) {
				if(set.value(KittySDK::Settings::S_PROFILE_PASSWORD).toString() == QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Sha1).toHex()) {
					qDebug() << "Password is ok too!";
					core->loadProfile(profile);
				} else {
					qWarning() << "The supplied password is wrong.";
				}
			} else {
				qWarning() << "Wanted to load profile" + profile + "but it's password protected!";
			}
		}
	}

	if(!core->profile()->isLoaded()) {
		qDebug() << "No profile, let's ask.";
		core->showProfilesWindow();
	}
}

void App::applySettings()
{
	Core *core = Core::inst();

	if(core->profile()) {
		core->profile()->loadIconTheme(core->setting(KittySDK::Settings::S_ICON_THEME).toString());
	}

	QNetworkProxy proxy;
	if(core->setting(KittySDK::Settings::S_PROXY_ENABLED, false).toBool()) {
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(core->setting(KittySDK::Settings::S_PROXY_SERVER).toString());
		proxy.setPort(core->setting(KittySDK::Settings::S_PROXY_PORT).toInt());

		if(core->setting(KittySDK::Settings::S_PROXY_AUTH, false).toBool()) {
			proxy.setUser(core->setting(KittySDK::Settings::S_PROXY_USERNAME).toString());
			proxy.setPassword(core->setting(KittySDK::Settings::S_PROXY_PASSWORD).toString());
		}
	}

	QNetworkProxy::setApplicationProxy(proxy);
}

void App::retranslate()
{
	Core *core = Core::inst();

	if(!m_translator) {
		m_translator = new QTranslator(this);
	}

	if(!m_qtTranslator) {
		m_qtTranslator = new QTranslator(this);
	}

	QString locale = core->setting(KittySDK::Settings::S_LANGUAGE).toString();
	if(locale.isEmpty()) {
		locale = QLocale::system().name();
	}

	QString dir = applicationDirPath() + "/data/translations/";
	if(m_translator->load("kitty_" + locale, dir) && m_qtTranslator->load("qt_" + locale, dir)) {
		installTranslator(m_translator);
		installTranslator(m_qtTranslator);
	} else {
		m_translator->load("");
		m_qtTranslator->load("");
	}
}

void App::cleanUp()
{
	Core *core = Core::inst();

	if(core->hasToRestart()) {
		QStringList args = core->appArguments();

		//remove executable name
		if(args.first().indexOf("Kitty") > -1) {
			args.removeFirst();
		}

		if(args.indexOf("-profile") == -1) {
			args.append("-profile");
			args.append(core->profileName());
		}

		QProcess::startDetached(qApp->applicationFilePath(), args);
	}

	Core::destr();
}

}
