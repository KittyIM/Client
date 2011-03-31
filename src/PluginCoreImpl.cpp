#include "PluginCoreImpl.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/MainWindow.h"
#include "SDK/SettingPage.h"
#include "AccountManager.h"
#include "IconManager.h"
#include "Core.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>

QVariant Kitty::PluginCoreImpl::setting(const QString &key, const QVariant &defaultValue)
{
  return Kitty::Core::inst()->setting(key, defaultValue);
}

void Kitty::PluginCoreImpl::setSetting(const QString &key, const QVariant &value)
{
  Kitty::Core::inst()->setSetting(key, value);
}

void Kitty::PluginCoreImpl::addSettingPage(KittySDK::SettingPage *page, const QString &parent)
{
  Kitty::Core::inst()->settingsWindow()->addPage(page, parent);
}

void Kitty::PluginCoreImpl::addToolbarAction(const QString &tb, QAction *action)
{
  Kitty::Core::inst()->mainWindow()->addToolbarAction(tb, action);
}

void Kitty::PluginCoreImpl::addAccount(KittySDK::Account *account)
{
  Kitty::AccountManager::inst()->add(account);
}

QPixmap Kitty::PluginCoreImpl::icon(const QString &id)
{
  return IconManager::inst()->icon(id);
}

void Kitty::PluginCoreImpl::addIcon(const QString &id, const QPixmap &pixmap, bool replace)
{
  IconManager::inst()->insert(id, pixmap, replace);
}
