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

PluginCoreImpl::PluginCoreImpl(Core *core):
	KittySDK::IPluginCore(core),
	m_core(core)
{
	connect(m_core->pluginManager(), SIGNAL(allLoaded()), SIGNAL(allPluginsLoaded()));
	connect(m_core->accountManager(), SIGNAL(allLoaded()), SIGNAL(allAccountsLoaded()));
	connect(m_core->contactManager(), SIGNAL(allLoaded()), SIGNAL(allContactsLoaded()));

	connect(m_core->accountManager(), SIGNAL(aboutToSave()), SIGNAL(accountsAboutToSave()));
	connect(m_core->contactManager(), SIGNAL(aboutToSave()), SIGNAL(contactsAboutToSave()));
}

QVariant PluginCoreImpl::setting(const QString &key, const QVariant &defaultValue)
{
	return m_core->setting(key, defaultValue);
}

void PluginCoreImpl::setSetting(const QString &key, const QVariant &value)
{
	m_core->setSetting(key, value);
}

void PluginCoreImpl::execPluginAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args)
{
	m_core->pluginManager()->execAction(pluginId, name, args);
}

QString PluginCoreImpl::profileName()
{
	return m_core->profileName();
}

QString PluginCoreImpl::kittyDir()
{
	return qApp->applicationDirPath();
}

QString PluginCoreImpl::profilesDir()
{
	return m_core->profilesDir();
}

QString PluginCoreImpl::avatarPath(KittySDK::IContact *contact)
{
	return m_core->avatarPath(contact);
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
	m_core->settingsWindow()->addPage(page, parent);
}

void PluginCoreImpl::addToolbarAction(const QString &tb, QAction *action)
{
	m_core->mainWindow()->addToolbarAction(tb, action);
}

void PluginCoreImpl::addAccount(KittySDK::IAccount *account)
{
	m_core->accountManager()->add(account);
}

QAction *PluginCoreImpl::action(const QString &id)
{
	return m_core->actionManager()->action(id);
}

void PluginCoreImpl::addAction(const QString &id, QAction *action)
{
	m_core->actionManager()->insert(id, action);
}

void PluginCoreImpl::removeAction(const QString &id)
{
}

QPixmap PluginCoreImpl::icon(const QString &id)
{
	return m_core->iconManager()->icon(id);
}

void PluginCoreImpl::addIcon(const QString &id, const QPixmap &pixmap, bool replace)
{
	m_core->iconManager()->insert(id, pixmap, replace);
}

int PluginCoreImpl::contactCount()
{
	return m_core->contactManager()->contacts().count();
}

KittySDK::IContact *PluginCoreImpl::contact(const int &id)
{
	if((id >= 0) && (id < contactCount())) {
		return m_core->contactManager()->contacts().at(id);
	}

	return 0;
}

KittySDK::IContact *PluginCoreImpl::contact(const QString &protocol, const QString &account, const QString &uid)
{
	if(KittySDK::IAccount *acc = m_core->accountManager()->account(protocol, account)) {
		return m_core->contactManager()->contact(acc, uid);
	}

	return 0;
}

QList<KittySDK::IContact*> PluginCoreImpl::contacts(const QString &protocol, const QString &account)
{
	return m_core->contactManager()->contactsByAccount(account, protocol);
}

QList<KittySDK::IContact*> PluginCoreImpl::contacts(const QString &protocol)
{
	return m_core->contactManager()->contactsByProtocol(protocol);
}

QStringList PluginCoreImpl::plugins()
{
	QStringList result;
	QList<Plugin*> plugins = m_core->pluginManager()->plugins();

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
		return m_core->mainWindow()->buttonForAction(tb, action);
	} else {
		qWarning() << "buttonForAction() unsupported ToolBar" << tb;
	}

	return 0;
}

void PluginCoreImpl::removeIcon(const QString &id)
{
	m_core->iconManager()->remove(id);
}

QString PluginCoreImpl::kittyVersion()
{
	return QString("kitty.im ") + Constants::VERSION;
}

quint32 PluginCoreImpl::enqueueMsg(const KittySDK::IMessage &msg)
{
	return m_core->messageQueue()->enqueue(msg);
}

void PluginCoreImpl::archiveMsg(const KittySDK::IMessage &msg)
{
	m_core->archiveMessage(msg);
}

}
