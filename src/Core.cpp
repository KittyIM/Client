#include "Core.h"

#include "widgets/ProfilesWindow.h"
#include "widgets/SettingsWindow.h"
#include "widgets/DebugWindow.h"
#include "widgets/AboutWindow.h"
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
#include <QtGui/QDesktopServices>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QApplication>
#include <QtGui/QMenu>

Kitty::Core *Kitty::Core::m_inst = 0;

Kitty::Core::Core()
{
  m_iconManager = new IconManager(this);
  m_actionManager = new ActionManager(this);

  m_profilesWindow = 0;
  m_settingsWindow = 0;
  m_aboutWindow = 0;
  m_mainWindow = 0;
  m_trayIcon = 0;
  m_profile = 0;

  m_restart = false;
  m_portable = false;
}

Kitty::Core::~Core()
{
  //TODO!
  if(m_mainWindow) {
    delete m_mainWindow;
  }

  if(m_settingsWindow) {
    delete m_settingsWindow;
  }

  if(m_profilesWindow) {
    delete m_profilesWindow;
  }

  if(m_trayIcon) {
    m_trayIcon->hide();
    delete m_trayIcon;
  }

  delete m_actionManager;
  delete m_iconManager;

  if(m_profile) {
    delete m_profile;
  }

  DebugWindow::destroy();
}

Kitty::Core* Kitty::Core::inst()
{
  static QMutex mutex;

  if(!m_inst) {
    mutex.lock();
    m_inst = new Core();
    mutex.unlock();
  }

  return m_inst;
}

void Kitty::Core::destroy()
{
  static QMutex mutex;

  if(m_inst) {
    mutex.lock();
    delete m_inst;
    m_inst = 0;
    mutex.unlock();
  }
}

QAction *Kitty::Core::action(const QString &id) const
{
  return m_actionManager->action(id);
}

QPixmap Kitty::Core::icon(const QString &id) const
{
  return m_iconManager->icon(id);
}

QVariant Kitty::Core::setting(const QString &key, const QVariant &defaultValue)
{
  return settings()->value(key, defaultValue);
}

void Kitty::Core::setSetting(const QString &key, const QVariant &value)
{
  return settings()->setValue(key, value);
}

void Kitty::Core::loadProfile(const QString &name)
{
  profile()->load(name);

  DebugWindow::inst()->restoreGeometry(setting(KittySDK::Settings::S_DEBUGWINDOW_GEOMETRY).toByteArray());

  mainWindow();

  qDebug() << "Profile " + name + " loaded!";
}

Kitty::MainWindow *Kitty::Core::mainWindow()
{
  if(!m_mainWindow) {
    m_actionManager->loadDefaults();
    m_mainWindow = new Kitty::MainWindow();
  }

  return m_mainWindow;
}

Kitty::AboutWindow *Kitty::Core::aboutWindow()
{
  if(!m_aboutWindow) {
    //m_mngrAct->loadDefaults();
    m_aboutWindow = new Kitty::AboutWindow();
  }

  return m_aboutWindow;
}

Kitty::ProfilesWindow *Kitty::Core::profilesWindow()
{
  if(!m_profilesWindow) {
    m_profilesWindow = new Kitty::ProfilesWindow();
  }

  return m_profilesWindow;
}

Kitty::SettingsWindow *Kitty::Core::settingsWindow()
{
  if(!m_settingsWindow) {
    m_settingsWindow = new Kitty::SettingsWindow();
    connect(m_settingsWindow, SIGNAL(settingsApplied()), mainWindow(), SLOT(applySettings()));
  }

  return m_settingsWindow;
}

void Kitty::Core::showMainWindow()
{
  mainWindow()->show();
}

void Kitty::Core::showProfilesWindow()
{
  profilesWindow()->show();
}

void Kitty::Core::showSettingsWindow()
{
  settingsWindow()->show();
}

QSystemTrayIcon *Kitty::Core::trayIcon()
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

Kitty::Profile *Kitty::Core::profile()
{
  if(!m_profile) {
    m_profile = new Kitty::Profile(this);
  }

  return m_profile;
}

Kitty::XmlSettings *Kitty::Core::settings()
{
  return profile()->settings();
}

QString Kitty::Core::profilesDir() const
{
  if(isPortable()) {
    return qApp->applicationDirPath() + "/profiles/";
  } else {
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/profiles/";
  }
}

void Kitty::Core::showTrayIcon()
{
  trayIcon()->show();
}

void Kitty::Core::showAboutWindow()
{
  aboutWindow()->show();
}

void Kitty::Core::restart()
{
  setRestart(true);
  qApp->quit();
}

void Kitty::Core::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if(reason == QSystemTrayIcon::Trigger) {
    action(KittySDK::Actions::A_SHOW_HIDE)->trigger();
  }
}

void Kitty::Core::toggleMainWindow()
{
  if(mainWindow()->isVisible()) {
    if(mainWindow()->isObscured()) {
      mainWindow()->activateWindow();
      mainWindow()->raise();
    } else {
    mainWindow()->hide();
    }
  } else {
    mainWindow()->show();
    mainWindow()->activateWindow();
    mainWindow()->raise();
  }
}
