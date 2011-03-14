#include "App.h"

#include "widgets/DebugWindow.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QTextCodec>
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
    Profile pro;
    pro.load(profile, true);

    if(!pro.hasPassword()) {
      core->loadProfile(profile);
    } else {
      qDebug() << "Wanted to load profile " + profile + " but it's password protected!";
    }
  }

  if(!core->profile()->isLoaded()) {
    core->showProfilesWindow();
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
