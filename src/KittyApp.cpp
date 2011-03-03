#include "KittyApp.h"

#include "KittyCore.h"
#include "KittyProfile.h"
#include "wndDebug.h"

#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QTextCodec>

KittyApp::KittyApp(int &argc, char **argv): QApplication(argc, argv)
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  qInstallMsgHandler(wndDebug::addMessage);
  setQuitOnLastWindowClosed(false);

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  KittyCore *core = KittyCore::inst();
  wndDebug::inst();

  QListIterator<QString> it(arguments());
  while(it.hasNext()) {
    QString arg = it.next();
    if(arg == "-debug") {
      wndDebug::inst()->show();
    } else if(arg == "-profile") {
      if(it.hasNext()) {
        QString profile = it.next();
        core->loadProfile(profile);
      }
    }
  }

  if(core->profile()->isLoaded()) {
    core->showMainWindow();
  } else {
    core->showProfilesWindow();
  }
}

void KittyApp::cleanUp()
{
  KittyCore *core = KittyCore::inst();

  if(core->hasToRestart()) {
    QProcess::startDetached(qApp->applicationFilePath());
  }

  KittyCore::destroy();
}
