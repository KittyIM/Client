#include "Core.h"

#include "widgets/windows/ProfilesWindow.h"
#include "widgets/windows/SettingsWindow.h"
#include "3rdparty/hunspell/hunspell.hxx"
#include "widgets/windows/DebugWindow.h"
#include "widgets/windows/AboutWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/windows/MainWindow.h"
#include "PluginManager.h"
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
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QApplication>
#include <QtGui/QMenu>

Kitty::Core::Core()
{
  m_profilesWindow = 0;
  m_settingsWindow = 0;
  m_aboutWindow = 0;
  m_chatWindow = 0;
  m_mainWindow = 0;
  m_trayIcon = 0;
  m_hunspell = 0;
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

  if(m_chatWindow) {
    delete m_chatWindow;
  }

  if(m_profilesWindow) {
    delete m_profilesWindow;
  }

  if(m_trayIcon) {
    delete m_trayIcon;
  }

  ActionManager::destr();
  IconManager::destr();

  if(m_profile) {
    delete m_profile;
  }

  DebugWindow::destroy();
}

QAction *Kitty::Core::action(const QString &id) const
{
  return ActionManager::inst()->action(id);
}

QPixmap Kitty::Core::icon(const QString &id) const
{
  return IconManager::inst()->icon(id);
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

  //mainWindow();
}

Kitty::MainWindow *Kitty::Core::mainWindow()
{
  if(!m_mainWindow) {
    m_mainWindow = new Kitty::MainWindow();
  }

  return m_mainWindow;
}

Kitty::ChatWindow *Kitty::Core::chatWindow()
{
  if(!m_chatWindow) {
    m_chatWindow = new Kitty::ChatWindow();
  }

  return m_chatWindow;
}

Kitty::AboutWindow *Kitty::Core::aboutWindow()
{
  if(!m_aboutWindow) {
    ActionManager::inst()->loadDefaults();
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
  }

  return m_settingsWindow;
}

void Kitty::Core::showMainWindow()
{
  mainWindow()->show();
}

void Kitty::Core::showChatWindow()
{
  chatWindow()->show();
}

void Kitty::Core::showProfilesWindow()
{
  profilesWindow()->show();
}

void Kitty::Core::showSettingsWindow()
{
  settingsWindow()->show();
  settingsWindow()->activateWindow();
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

Hunspell *Kitty::Core::hunspell()
{
  if(!m_hunspell) {
    QByteArray dic = QString(qApp->applicationDirPath() + "/data/dictionaries/pl_PL.dic").toLocal8Bit();
    QByteArray aff = QString(qApp->applicationDirPath() + "/data/dictionaries/pl_PL.aff").toLocal8Bit();

    m_hunspell = new Hunspell(aff.constData(), dic.constData());
  }

  return m_hunspell;
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

void Kitty::Core::openKittyFolder()
{
  QDesktopServices::openUrl(QUrl(qApp->applicationDirPath()));
}

void Kitty::Core::openProfilesFolder()
{
  QDesktopServices::openUrl(QUrl(profilesDir()));
}
