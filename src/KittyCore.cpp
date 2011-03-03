#include "KittyCore.h"

#include "wndMain.h"
#include "wndDebug.h"
#include "constants.h"
#include "wndProfiles.h"
#include "KittyProfile.h"
#include "SDK/constants.h"
#include "KittyIconMngr.h"
#include "KittyActionMngr.h"

#ifdef Q_WS_WIN32
  #include <windows.h>
  #include <winuser.h>
#endif

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QSystemTrayIcon>

KittyCore *KittyCore::m_inst = 0;

KittyCore::KittyCore()
{
  m_wndMain = 0;
  m_wndProfiles = 0;
  m_trayIcon = 0;
  m_profile = 0;
  m_mngrIcon = new KittyIconMngr(this);
  m_mngrAct = new KittyActionMngr(this);
  m_restart = false;
}

KittyCore::~KittyCore()
{
  if(m_wndMain) {
    delete m_wndMain;
  }

  if(m_wndProfiles) {
    delete m_wndProfiles;
  }

  if(m_trayIcon) {
    m_trayIcon->hide();
    delete m_trayIcon;
  }

  delete m_mngrAct;
  delete m_mngrIcon;

  if(m_profile) {
    delete m_profile;
  }

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

QAction *KittyCore::action(const char *id)
{
  return m_mngrAct->action(id);
}

QAction *KittyCore::action(const QString &id)
{
  return m_mngrAct->action(id);
}

QPixmap KittyCore::icon(const int &id)
{
  return m_mngrIcon->icon(id);
}

void KittyCore::loadProfile(QString name)
{
  profile()->load(name);
}

wndMain *KittyCore::mainWindow()
{
  if(!m_wndMain) {
    m_mngrAct->loadDefaults();
    m_wndMain = new wndMain();
  }

  return m_wndMain;
}

wndProfiles *KittyCore::profilesWindow()
{
  if(!m_wndProfiles) {
    m_wndProfiles = new wndProfiles();
  }

  return m_wndProfiles;
}

void KittyCore::showMainWindow()
{
  mainWindow()->show();
}

void KittyCore::showProfilesWindow()
{
  profilesWindow()->show();
}

QSystemTrayIcon *KittyCore::trayIcon()
{
  if(!m_trayIcon) {
    m_trayIcon = new QSystemTrayIcon(icon(KittySDK::Icons::KITTY));
    m_trayIcon->setToolTip(QString("KittyIM v%1").arg(Constants::VERSION));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    QMenu *menu = new QMenu();
    menu->addAction(action(KittySDK::Actions::SHOW_HIDE));
    menu->addSeparator();
    menu->addAction(action(KittySDK::Actions::SETTINGS));
    menu->addAction(action(KittySDK::Actions::QUIT));
    m_trayIcon->setContextMenu(menu);
  }

  return m_trayIcon;
}

KittyProfile *KittyCore::profile()
{
  if(!m_profile) {
    m_profile = new KittyProfile(this);
  }

  return m_profile;
}

void KittyCore::showTrayIcon()
{
  trayIcon()->show();
}

void KittyCore::restart()
{
  setRestart(true);
  qApp->quit();
}

void KittyCore::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if(reason == QSystemTrayIcon::Trigger) {
    action(KittySDK::Actions::SHOW_HIDE)->trigger();
  }
}

void KittyCore::toggleMainWindow()
{
  if(mainWindow()->isVisible()) {
    mainWindow()->hide();
    mainWindow()->activateWindow();
    mainWindow()->raise();
  } else {
    mainWindow()->show();
  }
}
