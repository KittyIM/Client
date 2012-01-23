#include "PluginCoreImpl.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/MainWindow.h"
#include "3rdparty/json/json.h"
#include "SDK/SettingPage.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "PluginManager.h"
#include "SDK/constants.h"
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

QString Kitty::PluginCoreImpl::avatarPath(Contact *contact)
{
	return Core::inst()->avatarPath(contact);
}

QVariant Kitty::PluginCoreImpl::jsonParse(const QString &json)
{
	return Json::parse(json);
}

QString Kitty::PluginCoreImpl::jsonStringify(const QVariant &json, int indent)
{
	return Json::stringify(json, indent);
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

int Kitty::PluginCoreImpl::contactCount()
{
	return ContactManager::inst()->contacts().count();
}

Contact *Kitty::PluginCoreImpl::contact(const int &id)
{
	if((id >= 0) && (id < contactCount())) {
		return ContactManager::inst()->contacts().at(id);
	}

	return 0;
}

QList<Contact*> Kitty::PluginCoreImpl::contacts(const QString &account, const QString &protocol)
{
	return ContactManager::inst()->contactsByAccount(account, protocol);
}

QList<KittySDK::Contact*> Kitty::PluginCoreImpl::contacts(const QString &protocol)
{
	return ContactManager::inst()->contactsByProtocol(protocol);
}

QStringList Kitty::PluginCoreImpl::plugins()
{
	QStringList result;
	QList<Plugin*> plugins = PluginManager::inst()->plugins();

	foreach(Plugin *plug, plugins) {
		result.append(plug->plugin()->info()->name());
	}

	return result;
}

void Kitty::PluginCoreImpl::removeSettingPage(SettingPage *page)
{

}

void Kitty::PluginCoreImpl::removeToolbarAction(const QString &tb, QAction *action)
{

}

QToolButton *Kitty::PluginCoreImpl::buttonForAction(const QString &tb, QAction *action)
{
	if((tb == Toolbars::TB_MAIN) || (tb == Toolbars::TB_PLUGINS) || (tb == Toolbars::TB_NETWORKS)) {
		return Core::inst()->mainWindow()->buttonForAction(tb, action);
	} else {
		qWarning() << "buttonForAction() unsupported ToolBar" << tb;
	}

	return 0;
}

void Kitty::PluginCoreImpl::removeIcon(const QString &id)
{
	IconManager::inst()->remove(id);
}
