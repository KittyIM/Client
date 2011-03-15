#include "App.h"

#include "widgets/windows/DebugWindow.h"
#include "SDK/constants.h"
#include "XmlSettings.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QFile>

Kitty::App::App(int &argc, char **argv): QApplication(argc, argv)
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QCoreApplication::setApplicationName("KittyIM");

  qInstallMsgHandler(DebugWindow::addMessage);
  setQuitOnLastWindowClosed(false);

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  Core *core = Core::inst();
  DebugWindow::inst();

  QString profile;
  QListIterator<QString> it(arguments());
  while(it.hasNext()) {
    QString arg = it.next();
    if(arg == "-debug") {
      DebugWindow::inst()->show();
    } else if(arg == "-profile") {
      if(it.hasNext()) {
        profile = it.next();
      }
    } else if(arg == "-portable") {
      core->setPortable(true);
    }
  }

  if(!core->isPortable()) {
    if(QFile::exists(applicationDirPath() + "/data/portable")) {
      core->setPortable(true);
    }
  }

  //TODO: When only 1 profile with no password exists, skip profile selection
  if(!profile.isEmpty()) {
    Kitty::XmlSettings set(Kitty::Core::inst()->profilesDir() + "/" + profile);
    bool hasPassword = !set.value(KittySDK::Settings::S_PROFILE_PASSWORD).toString().isEmpty();

    if(!hasPassword) {
      core->loadProfile(profile);
    } else {
      qDebug() << "Wanted to load profile " + profile + " but it's password protected!";
    }
  }

  if(!core->profile()->isLoaded()) {
    core->showProfilesWindow();
  }
}

void Kitty::App::applySettings()
{
  QTranslator *translator = new QTranslator();
  QTranslator *qtTranslator = new QTranslator();

  QString locale = QLocale::system().name();
  locale = Core::inst()->setting(KittySDK::Settings::S_LANGUAGE, locale).toString();

  QString dir = applicationDirPath() + "/data/translations/";
  if(translator->load("kitty_" + locale, dir) && qtTranslator->load("qt_" + locale, dir)) {
    installTranslator(translator);
    installTranslator(qtTranslator);
  } else {
    translator->load(QString());
  }
}

void Kitty::App::cleanUp()
{
  Core *core = Core::inst();

  if(core->hasToRestart()) {
    QProcess::startDetached(qApp->applicationFilePath());
  }

  Core::destr();
}
