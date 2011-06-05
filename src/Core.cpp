#include "Core.h"

#include "widgets/windows/ProfilesWindow.h"
#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/ContactWindow.h"
#include "../lib/hunspell/hunspell.hxx"
#include "widgets/windows/DebugWindow.h"
#include "widgets/windows/AboutWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/windows/MainWindow.h"
#include "ProtocolManager.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "PluginManager.h"
#include "ActionManager.h"
#include "SDK/constants.h"
#include "JsonSettings.h"
#include "ChatManager.h"
#include "IconManager.h"
#include "constants.h"
#include "Profile.h"

#ifdef Q_WS_WIN32
  #include <windows.h>
  #include <winuser.h>
#endif

#include <QtCore/QCryptographicHash>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QApplication>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[Core]"
#define qWarning() qWarning() << "[Core]"

using namespace Kitty;
using namespace KittySDK;

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

  if(m_hunspell) {
    delete m_hunspell;
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

  ChatManager::destr();
  ContactManager::destr();
  AccountManager::destr();

  ProtocolManager::destr();
  PluginManager::destr();

  DebugWindow::destroy();
}

QString Kitty::Core::statusToString(const int &status)
{
  switch(status) {
    case Protocol::Online:
      return tr("Online");
    break;

    case Protocol::Away:
      return tr("Away");
    break;

    case Protocol::FFC:
      return tr("Free for chat");
    break;

    case Protocol::DND:
      return tr("Do not disturb");
    break;

    case Protocol::Invisible:
      return tr("Invisible");
    break;

    case Protocol::Offline:
      return tr("Offline");
    break;
  }

  return tr("Unknown");
}

QString Kitty::Core::processUrls(const QString &text)
{
  QString result = text;

  QRegExp urls("\\b((?:(?:https?|ftp|file)://|www\\.|ftp\\.))([-A-Z0-9+&@#/%=~_|$?!:,.]*[A-Z0-9+&@#/%=~_|$])", Qt::CaseInsensitive);
  int pos = 0;
  while((pos = urls.indexIn(result, pos)) != -1) {
    QString url = urls.cap(0);

    if(urls.cap(1) == "www.") {
      url.prepend("http://");
    } else if(urls.cap(1) == "ftp.") {
      url.prepend("ftp://");
    }

    QString link = QString("<a href=\"%1\">%2</a>").arg(url).arg(urls.cap(0));
    result.replace(pos, urls.cap(0).length(), link);
    pos += link.size();
  }

  QRegExp emails("[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?", Qt::CaseInsensitive);
  pos = 0;
  while((pos = emails.indexIn(result, pos)) != -1) {
    QString mail = emails.cap(0);
    QString link = QString("<a href=\"mailto:%1\">%1</a>").arg(mail);
    result.replace(pos, mail.length(), link);
    pos += link.size();
  }

  return result;
}

QString Kitty::Core::processEmoticons(const QString &text)
{
  return text;
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

  DebugWindow::inst()->restoreGeometry(setting(Settings::S_DEBUGWINDOW_GEOMETRY).toByteArray());

  mainWindow();
}

MainWindow *Kitty::Core::mainWindow()
{
  if(!m_mainWindow) {
    m_mainWindow = new MainWindow();
  }

  return m_mainWindow;
}

ChatWindow *Kitty::Core::chatWindow()
{
  if(!m_chatWindow) {
    m_chatWindow = new ChatWindow();
  }

  return m_chatWindow;
}

AboutWindow *Kitty::Core::aboutWindow()
{
  if(!m_aboutWindow) {
    ActionManager::inst()->loadDefaults();
    m_aboutWindow = new AboutWindow();
  }

  return m_aboutWindow;
}

ProfilesWindow *Kitty::Core::profilesWindow()
{
  if(!m_profilesWindow) {
    m_profilesWindow = new ProfilesWindow();
  }

  return m_profilesWindow;
}

SettingsWindow *Kitty::Core::settingsWindow()
{
  if(!m_settingsWindow) {
    m_settingsWindow = new SettingsWindow();
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

void Kitty::Core::showAddContactWindow()
{
  ContactWindow *wnd = new ContactWindow();
  wnd->show();
}

QSystemTrayIcon *Kitty::Core::trayIcon()
{
  if(!m_trayIcon) {
    m_trayIcon = new QSystemTrayIcon(icon(Icons::I_KITTY));
    m_trayIcon->setToolTip(QString("KittyIM v%1").arg(Constants::VERSION));
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    QMenu *menu = new QMenu(m_mainWindow);
    menu->addAction(action(Actions::A_SHOW_HIDE));
    menu->addSeparator();
    menu->addAction(action(Actions::A_SETTINGS));
    menu->addAction(action(Actions::A_QUIT));
    m_trayIcon->setContextMenu(menu);
  }

  return m_trayIcon;
}

Profile *Kitty::Core::profile()
{
  if(!m_profile) {
    m_profile = new Profile(this);
  }

  return m_profile;
}

Hunspell *Kitty::Core::hunspell()
{
  if(!m_hunspell) {
    QByteArray dic = QString(qApp->applicationDirPath() + "/data/dictionaries/" + setting(Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString() + ".dic").toLocal8Bit();
    QByteArray aff = QString(qApp->applicationDirPath() + "/data/dictionaries/" + setting(Settings::S_CHATWINDOW_SPELLCHECK_DICT).toString() + ".aff").toLocal8Bit();

    m_hunspell = new Hunspell(aff.constData(), dic.constData());
  }

  return m_hunspell;
}

JsonSettings *Kitty::Core::settings()
{
  return profile()->settings();
}

QString Kitty::Core::avatarPath(KittySDK::Contact *contact)
{
  if(contact) {
    Protocol *proto = contact->protocol();
    if(proto) {
      return currentProfileDir() + "avatars/" + QCryptographicHash::hash(QString("avatar_" + proto->protoInfo()->protoName() + "_" + contact->uid()).toAscii(), QCryptographicHash::Md5).toHex() + ".png";
    }
  }

  return "";
}
QString Kitty::Core::profilesDir() const
{
  if(isPortable()) {
    return qApp->applicationDirPath() + "/profiles/";
  } else {
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/profiles/";
  }
}

QString Kitty::Core::currentProfileDir()
{
  return profilesDir() + profile()->name() + "/";
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
    action(Actions::A_SHOW_HIDE)->trigger();
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

// function taken from http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
bool Kitty::Core::removeDir(const QString &dirName)
{
  bool result = true;
  QDir dir(dirName);

  if(dir.exists(dirName)) {
    foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
      if(info.isDir()) {
        result = removeDir(info.absoluteFilePath());
      } else {
        result = QFile::remove(info.absoluteFilePath());
      }

      if(!result) {
        return result;
      }
    }

    result = dir.rmdir(dirName);
  }

  return result;
}

