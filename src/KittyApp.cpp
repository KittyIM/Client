#include "KittyApp.h"
#include "KittyCore.h"
#include "wndDebug.h"

#include <QtCore/QDebug>
#include <QtCore/QTextCodec>

KittyApp::KittyApp(int &argc, char **argv): QApplication(argc, argv)
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  qInstallMsgHandler(wndDebug::addMessage);
  setQuitOnLastWindowClosed(false);

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(slotCleanUp()));

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
        qDebug() << profile;
      }
    }
  }

  core->showWndMain();
}

void KittyApp::slotCleanUp()
{
  KittyCore::destroy();
}
