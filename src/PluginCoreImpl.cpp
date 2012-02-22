#include "PluginCoreImpl.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/MainWindow.h"
#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "ActionManager.h"
#include "PluginManager.h"
#include "IconManager.h"
#include "Profile.h"
#include "Core.h"

#include <ISettingsPage.h>
#include <SDKConstants.h>

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtGui/QApplication>

namespace Kitty
{

PluginCoreImpl::PluginCoreImpl(QObject *parent)
{
	connect(PluginManager::inst(), SIGNAL(allPluginsLoaded()), this, SIGNAL(allPluginsLoaded()));
}

QVariant PluginCoreImpl::setting(const QString &key, const QVariant &defaultValue)
{
	return Core::inst()->setting(key, defaultValue);
}

void PluginCoreImpl::setSetting(const QString &key, const QVariant &value)
{
	Core::inst()->setSetting(key, value);
}

void PluginCoreImpl::execPluginAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args)
{
	PluginManager::inst()->execAction(pluginId, name, args);
}

QString PluginCoreImpl::profileName()
{
	return Core::inst()->profileName();
}

QString PluginCoreImpl::kittyDir()
{
	return qApp->applicationDirPath();
}

QString PluginCoreImpl::profilesDir()
{
	return Core::inst()->profilesDir();
}

QString PluginCoreImpl::avatarPath(KittySDK::IContact *contact)
{
	return Core::inst()->avatarPath(contact);
}

QVariant PluginCoreImpl::jsonParse(const QString &json)
{
	return Json::parse(json);
}

QString PluginCoreImpl::jsonStringify(const QVariant &json, int indent)
{
	return Json::stringify(json, indent);
}

void PluginCoreImpl::addSettingPage(KittySDK::ISettingsPage *page, const QString &parent)
{
	Core::inst()->settingsWindow()->addPage(page, parent);
}

void PluginCoreImpl::addToolbarAction(const QString &tb, QAction *action)
{
	Core::inst()->mainWindow()->addToolbarAction(tb, action);
}

void PluginCoreImpl::addAccount(KittySDK::IAccount *account)
{
	AccountManager::inst()->add(account);
}

QAction *PluginCoreImpl::action(const QString &id)
{
	return ActionManager::inst()->action(id);
}

void PluginCoreImpl::addAction(const QString &id, QAction *action)
{
	ActionManager::inst()->insert(id, action);
}

void PluginCoreImpl::removeAction(const QString &id)
{
}

QPixmap PluginCoreImpl::icon(const QString &id)
{
	return IconManager::inst()->icon(id);
}

void PluginCoreImpl::addIcon(const QString &id, const QPixmap &pixmap, bool replace)
{
	IconManager::inst()->insert(id, pixmap, replace);
}

int PluginCoreImpl::contactCount()
{
	return ContactManager::inst()->contacts().count();
}

KittySDK::IContact *PluginCoreImpl::contact(const int &id)
{
	if((id >= 0) && (id < contactCount())) {
		return ContactManager::inst()->contacts().at(id);
	}

	return 0;
}

QList<KittySDK::IContact*> PluginCoreImpl::contacts(const QString &account, const QString &protocol)
{
	return ContactManager::inst()->contactsByAccount(account, protocol);
}

QList<KittySDK::IContact*> PluginCoreImpl::contacts(const QString &protocol)
{
	return ContactManager::inst()->contactsByProtocol(protocol);
}

QStringList PluginCoreImpl::plugins()
{
	QStringList result;
	QList<Plugin*> plugins = PluginManager::inst()->plugins();

	foreach(Plugin *plug, plugins) {
		if(KittySDK::IPlugin *iplugin = plug->iplugin()) {
			if(KittySDK::IPluginInfo *info = iplugin->info()) {
				result.append(info->id());
			}
		}
	}

	return result;
}

void PluginCoreImpl::removeSettingPage(KittySDK::ISettingsPage *page)
{

}

void PluginCoreImpl::removeToolbarAction(const QString &tb, QAction *action)
{

}

QToolButton *PluginCoreImpl::buttonForAction(const QString &tb, QAction *action)
{
	if((tb == KittySDK::Toolbars::TB_MAIN) || (tb == KittySDK::Toolbars::TB_PLUGINS) || (tb == KittySDK::Toolbars::TB_NETWORKS)) {
		return Core::inst()->mainWindow()->buttonForAction(tb, action);
	} else {
		qWarning() << "buttonForAction() unsupported ToolBar" << tb;
	}

	return 0;
}

void PluginCoreImpl::removeIcon(const QString &id)
{
	IconManager::inst()->remove(id);
}

}
