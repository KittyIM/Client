#include "App.h"

#include "widgets/DebugWindow.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QTextCodec>
#include <QtCore/QProcess>
#include <QtCore/QDebug>

using namespace Kitty;

App::App(int &argc, char **argv): QApplication(argc, argv)
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  qInstallMsgHandler(DebugWindow::addMessage);
  setQuitOnLastWindowClosed(false);

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  Core *core = Core::inst();
  DebugWindow::inst();

  QListIterator<QString> it(arguments());
  while(it.hasNext()) {
    QString arg = it.next();
    if(arg == "-debug") {
      DebugWindow::inst()->show();
    } else if(arg == "-profile") {
      if(it.hasNext()) {
        QString profile = it.next();
        core->loadProfile(profile);
      }
    }
  }

  if(!core->profile()->isLoaded()) {
    core->showProfilesWindow();
  }
}

void App::cleanUp()
{
  Core *core = Core::inst();

  if(core->hasToRestart()) {
    QProcess::startDetached(qApp->applicationFilePath());
  }

  Core::destroy();
}
