#include "Core.h"

#include "widgets/ProfilesWindow.h"
#include "widgets/DebugWindow.h"
#include "widgets/MainWindow.h"
#include "ActionManager.h"
#include "SDK/constants.h"
#include "XmlSettings.h"
#include "IconManager.h"
#include "constants.h"
#include "Profile.h"

#ifdef Q_WS_WIN32
  #include <windows.h>
  #include <winuser.h>
#endif

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QApplication>
#include <QtGui/QMenu>

using namespace Kitty;

Core *Core::m_inst = 0;

Core::Core()
{
  m_wndMain = 0;
  m_wndProfiles = 0;
  m_trayIcon = 0;
  m_profile = 0;
  m_mngrIcon = new IconManager(this);
  m_mngrAct = new ActionManager(this);
  m_restart = false;
}

Core::~Core()
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

  DebugWindow::destroy();
}

Core* Core::inst()
{
  static QMutex mutex;

  if(!m_inst) {
    mutex.lock();
    m_inst = new Core();
    mutex.unlock();
  }

  return m_inst;
}

void Core::destroy()
{
  static QMutex mutex;

  if(m_inst) {
    mutex.lock();
    delete m_inst;
    m_inst = 0;
    mutex.unlock();
  }
}

QAction *Core::action(const QString &id)
{
  return m_mngrAct->action(id);
}

QPixmap Core::icon(const QString &id)
{
  return m_mngrIcon->icon(id);
}

QVariant Core::setting(const QString &key, const QVariant &defaultValue)
{
  return settings()->value(key, defaultValue);
}

void Core::setSetting(const QString &key, const QVariant &value)
{
  return settings()->setValue(key, value);
}

void Core::loadProfile(QString name)
{
  profile()->load(name);

  DebugWindow::inst()->restoreGeometry(setting("Kitty.DebugWindow.Geometry").toByteArray());

  mainWindow();

  qDebug() << "Profile " + name + " loaded!";
}

MainWindow *Core::mainWindow()
{
  if(!m_wndMain) {
    m_mngrAct->loadDefaults();
    m_wndMain = new MainWindow();
  }

  return m_wndMain;
}

ProfilesWindow *Core::profilesWindow()
{
  if(!m_wndProfiles) {
    m_wndProfiles = new ProfilesWindow();
  }

  return m_wndProfiles;
}

void Core::showMainWindow()
{
  mainWindow()->show();
}

void Core::showProfilesWindow()
{
  profilesWindow()->show();
}

QSystemTrayIcon *Core::trayIcon()
{
  if(!m_trayIcon) {
    m_trayIcon = new QSystemTrayIcon(icon(KittySDK::Icons::I_KITTY));
    m_trayIcon->setToolTip(QString("KittyIM v%1").arg(Constants::VERSION));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    QMenu *menu = new QMenu();
    menu->addAction(action(KittySDK::Actions::A_SHOW_HIDE));
    menu->addSeparator();
    menu->addAction(action(KittySDK::Actions::A_SETTINGS));
    menu->addAction(action(KittySDK::Actions::A_QUIT));
    m_trayIcon->setContextMenu(menu);
  }

  return m_trayIcon;
}

Profile *Core::profile()
{
  if(!m_profile) {
    m_profile = new Profile(this);
  }

  return m_profile;
}

XmlSettings *Core::settings()
{
  return profile()->settings();
}

QString Core::profilesDir()
{
  if(true /*isPortable()*/) {
    return qApp->applicationDirPath() + "/profiles/";
  }
}

void Core::showTrayIcon()
{
  trayIcon()->show();
}

void Core::restart()
{
  setRestart(true);
  qApp->quit();
}

void Core::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if(reason == QSystemTrayIcon::Trigger) {
    action(KittySDK::Actions::A_SHOW_HIDE)->trigger();
  }
}

void Core::toggleMainWindow()
{
  if(mainWindow()->isVisible()) {
    mainWindow()->hide();
    mainWindow()->activateWindow();
    mainWindow()->raise();
  } else {
    mainWindow()->show();
  }
}
