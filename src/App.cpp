#include "App.h"

#include "widgets/windows/DebugWindow.h"
#include "SDK/constants.h"
#include "JsonSettings.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkProxy>

#define qDebug() qDebug() << "[App]"
#define qWarning() qWarning() << "[App]"

using namespace Kitty;
using namespace KittySDK;

Kitty::App::App(int &argc, char **argv): QApplication(argc, argv)
{
  m_startDate = QDateTime::currentDateTime();

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QCoreApplication::setApplicationName("KittyIM");

  qInstallMsgHandler(DebugWindow::addMessage);
  setQuitOnLastWindowClosed(false);

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  qDebug() << "Starting KittyIM";
  qDebug() << "There are" << (arguments().count() - 1) << "arguments";

  Core *core = Core::inst();

  QString profile;
  QListIterator<QString> it(arguments());
  while(it.hasNext()) {
    QString arg = it.next();
    if(arg == "-debug") {
      qDebug() << "-debug found, showing debug console";
      DebugWindow::inst()->show();
    } else if(arg == "-profile") {
      if(it.hasNext()) {
        profile = it.next();
        qDebug() << "-profile found, profile is" << profile;
      } else {
        qWarning() << "-profile found but nothing more";
      }
    } else if(arg == "-portable") {
      qDebug() << "-portable found, we are going portable";
      core->setPortable(true);
    }
  }

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
      JsonSettings set(Core::inst()->profilesDir() + profiles[0] + "/settings.dat");
      bool hasPassword = !set.value(Settings::S_PROFILE_PASSWORD).toString().isEmpty();
      if(!hasPassword) {
        qDebug() << "Only one profile with no password, load it!";
        core->loadProfile(profiles[0]);
      }
    }
  } else {
    JsonSettings set(Core::inst()->profilesDir() + profile + "/settings.dat");
    bool hasPassword = !set.value(Settings::S_PROFILE_PASSWORD).toString().isEmpty();

    if(!hasPassword) {
      qDebug() << "Profile is ok, loading.";
      core->loadProfile(profile);
    } else {
      qWarning() << "Wanted to load profile" + profile + "but it's password protected!";
    }
  }

  if(!core->profile()->isLoaded()) {
    qDebug() << "No profile, let's ask.";
    core->showProfilesWindow();
  }
}

void Kitty::App::applySettings()
{
  Core *core = Core::inst();

  QTranslator *translator = new QTranslator(this);
  QTranslator *qtTranslator = new QTranslator(this);

  QString locale = QLocale::system().name();
  locale = core->setting(Settings::S_LANGUAGE, locale).toString();

  QString dir = applicationDirPath() + "/data/translations/";
  if(translator->load("kitty_" + locale, dir) && qtTranslator->load("qt_" + locale, dir)) {
    installTranslator(translator);
    installTranslator(qtTranslator);
  } else {
    translator->load(QString());
  }

  QFile file(":/header/style.css");
  file.open(QFile::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();

  QNetworkProxy proxy;
  if(core->setting(Settings::S_PROXY_ENABLED, false).toBool()) {
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(core->setting(Settings::S_PROXY_SERVER).toString());
    proxy.setPort(core->setting(Settings::S_PROXY_PORT).toInt());

    if(core->setting(Settings::S_PROXY_AUTH, false).toBool()) {
      proxy.setUser(core->setting(Settings::S_PROXY_USERNAME).toString());
      proxy.setPassword(core->setting(Settings::S_PROXY_PASSWORD).toString());
    }
  }

  QNetworkProxy::setApplicationProxy(proxy);
}

void Kitty::App::cleanUp()
{
  Core *core = Core::inst();

  if(core->hasToRestart()) {
    QProcess::startDetached(qApp->applicationFilePath());
  }

  Core::destr();
}
