#include "PluginCoreImpl.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/MainWindow.h"
#include "SDK/SettingPage.h"
#include "PluginManager.h"
#include "AccountManager.h"
#include "IconManager.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtGui/QApplication>

using namespace Kitty;
using namespace KittySDK;

Kitty::PluginCoreImpl::PluginCoreImpl(QObject *parent)
{
  connect(PluginManager::inst(), SIGNAL(allPluginsLoaded()), this, SIGNAL(allPluginsLoaded()));
}

QVariant Kitty::PluginCoreImpl::setting(const QString &key, const QVariant &defaultValue)
{
  return Core::inst()->setting(key, defaultValue);
}

void Kitty::PluginCoreImpl::setSetting(const QString &key, const QVariant &value)
{
  Core::inst()->setSetting(key, value);
}

void Kitty::PluginCoreImpl::execPluginAction(const QString &plugin, const QString &name, const QMap<QString, QVariant> &args)
{
  PluginManager::inst()->execAction(plugin, name, args);
}

QString Kitty::PluginCoreImpl::profileName()
{
  return Core::inst()->profile()->name();
}

QString Kitty::PluginCoreImpl::kittyDir()
{
  return qApp->applicationDirPath();
}

QString Kitty::PluginCoreImpl::profilesDir()
{
  return Core::inst()->profilesDir();
}

void Kitty::PluginCoreImpl::addSettingPage(SettingPage *page, const QString &parent)
{
  Core::inst()->settingsWindow()->addPage(page, parent);
}

void Kitty::PluginCoreImpl::addToolbarAction(const QString &tb, QAction *action)
{
  Core::inst()->mainWindow()->addToolbarAction(tb, action);
}

void Kitty::PluginCoreImpl::addAccount(Account *account)
{
  AccountManager::inst()->add(account);
}

QPixmap Kitty::PluginCoreImpl::icon(const QString &id)
{
  return IconManager::inst()->icon(id);
}

void Kitty::PluginCoreImpl::addIcon(const QString &id, const QPixmap &pixmap, bool replace)
{
  IconManager::inst()->insert(id, pixmap, replace);
}
