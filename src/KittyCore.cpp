#include "KittyCore.h"

#include "wndMain.h"
#include "wndDebug.h"
#include "constants.h"
#include "SDK/constants.h"
#include "KittyIconMngr.h"
#include "KittyActionMngr.h"

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtGui/QMenu>
#include <QtGui/QSystemTrayIcon>

KittyCore *KittyCore::m_inst = 0;

KittyCore::KittyCore()
{
  m_wndMain = 0;
  m_trayIcon = 0;
  m_mngrIcon = new KittyIconMngr();
  m_mngrAct = new KittyActionMngr();
}

KittyCore::~KittyCore()
{
  if(m_wndMain) {
    delete m_wndMain;
  }

  if(m_trayIcon) {
    delete m_trayIcon;
  }

  delete m_mngrAct;
  delete m_mngrIcon;

  wndDebug::destroy();
}

KittyCore* KittyCore::inst()
{
  static QMutex mutex;

  if(!m_inst) {
    mutex.lock();
    m_inst = new KittyCore();
    mutex.unlock();
  }

  return m_inst;
}

void KittyCore::destroy()
{
  static QMutex mutex;

  if(m_inst) {
    mutex.lock();
    delete m_inst;
    m_inst = 0;
    mutex.unlock();
  }
}

QAction *KittyCore::getAction(const int &id)
{
  return m_mngrAct->getAction(id);
}

QPixmap KittyCore::getIcon(const int &id)
{
  return m_mngrIcon->getIcon(id);
}

wndMain *KittyCore::getWndMain()
{
  if(!m_wndMain) {
    m_mngrAct->loadDefaults();
    m_wndMain = new wndMain();
  }

  return m_wndMain;
}

void KittyCore::showWndMain()
{
  getWndMain()->show();
}

QSystemTrayIcon *KittyCore::getTrayIcon()
{
  if(!m_trayIcon) {
    m_trayIcon = new QSystemTrayIcon(getIcon(KittySDK::Icons::KITTY));
    m_trayIcon->setToolTip(QString("KittyIM v%1").arg(Constants::VERSION));

    QMenu *menu = new QMenu();
    menu->addAction(getAction(KittySDK::Actions::SHOW_HIDE));
    menu->addSeparator();
    menu->addAction(getAction(KittySDK::Actions::SETTINGS));
    menu->addAction(getAction(KittySDK::Actions::QUIT));
    m_trayIcon->setContextMenu(menu);
  }

  return m_trayIcon;
}

void KittyCore::showTrayIcon()
{
  getTrayIcon()->show();
}
