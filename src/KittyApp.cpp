#include "KittyApp.h"
#include "KittyCore.h"
#include "wndDebug.h"

KittyApp::KittyApp(int &argc, char **argv): QApplication(argc, argv)
{
  qInstallMsgHandler(wndDebug::addMessage);

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(slotCleanUp()));

  KittyCore *core = KittyCore::inst();

  wndDebug::inst()->show();

  core->showWndMain();
}

void KittyApp::slotCleanUp()
{
  KittyCore::destroy();
}
