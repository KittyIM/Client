#include "PluginCoreImpl.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/MainWindow.h"
#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "ActionManager.h"
#include "PluginManager.h"
#include "MessageQueue.h"
#include "IconManager.h"
#include "constants.h"
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
	connect(PluginManager::inst(), SIGNAL(allLoaded()), SIGNAL(allPluginsLoaded()));
	connect(Core::inst()->accountManager(), SIGNAL(allLoaded()), SIGNAL(allAccountsLoaded()));
	connect(Core::inst()->contactManager(), SIGNAL(allLoaded()), SIGNAL(allContactsLoaded()));

	connect(Core::inst()->accountManager(), SIGNAL(aboutToSave()), SIGNAL(accountsAboutToSave()));
	connect(Core::inst()->contactManager(), SIGNAL(aboutToSave()), SIGNAL(contactsAboutToSave()));
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
	Core::inst()->accountManager()->add(account);
}

QAction *PluginCoreImpl::action(const QString &id)
{
	return Core::inst()->actionManager()->action(id);
}

void PluginCoreImpl::addAction(const QString &id, QAction *action)
{
	Core::inst()->actionManager()->insert(id, action);
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
	return Core::inst()->contactManager()->contacts().count();
}

KittySDK::IContact *PluginCoreImpl::contact(const int &id)
{
	if((id >= 0) && (id < contactCount())) {
		return Core::inst()->contactManager()->contacts().at(id);
	}

	return 0;
}

KittySDK::IContact *PluginCoreImpl::contact(const QString &protocol, const QString &account, const QString &uid)
{
	if(KittySDK::IAccount *acc = Core::inst()->accountManager()->account(protocol, account)) {
		return Core::inst()->contactManager()->contact(acc, uid);
	}

	return 0;
}

QList<KittySDK::IContact*> PluginCoreImpl::contacts(const QString &protocol, const QString &account)
{
	return Core::inst()->contactManager()->contactsByAccount(account, protocol);
}

QList<KittySDK::IContact*> PluginCoreImpl::contacts(const QString &protocol)
{
	return Core::inst()->contactManager()->contactsByProtocol(protocol);
}

QStringList PluginCoreImpl::plugins()
{
	QStringList result;
	QList<Plugin*> plugins = PluginManager::inst()->plugins();

	foreach(Plugin *plug, plugins) {
		if(plug->hasError()) {
			continue;
		}

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

QString PluginCoreImpl::kittyVersion()
{
	return QString("kitty.im ") + Constants::VERSION;
}

quint32 PluginCoreImpl::enqueueMsg(const KittySDK::IMessage &msg)
{
	return MessageQueue::inst()->enqueue(msg);
}

void PluginCoreImpl::archiveMsg(const KittySDK::IMessage &msg)
{
	Core::inst()->archiveMessage(msg);
}

}
