#include "PluginManager.h"

#include "widgets/windows/SettingsWindow.h"
#include "ProtocolManager.h"
#include "PluginCoreImpl.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IProtocol.h>

#include <QtCore/QTranslator>
#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[PluginManager]"
#define qWarning() qWarning() << "[PluginManager]"

namespace Kitty
{

typedef QObject *(*pluginInst)(KittySDK::IPluginCore*);

Plugin::Plugin(const QString &fileName):
	m_fileName(fileName),
	m_translator(0)
{
	m_loaded = false;
	m_inited = false;

	QLibrary lib(m_fileName);
	pluginInst inst = (pluginInst)lib.resolve("inst");

	if(inst) {
		if((m_plugin = dynamic_cast<KittySDK::IPlugin*>(inst(new PluginCoreImpl())))) {
			if(m_plugin->type() == KittySDK::IPlugin::Type) {
				//nothing for now
			} else if(m_plugin->type() == KittySDK::IProtocol::Type) {
				if(KittySDK::IProtocol *prot = dynamic_cast<KittySDK::IProtocol*>(m_plugin)) {
					ProtocolManager::inst()->add(prot);
				} else {
					qWarning() << "Could not cast to protocol for file" << QFileInfo(m_fileName).fileName();
				}
			} else {
				qWarning() << "Unknown type for file" << QFileInfo(m_fileName).fileName();
			}
		} else {
			qWarning() << "Could not cast to Plugin for file" << QFileInfo(m_fileName).fileName();
		}
	} else {
		qWarning() << "Resolve failed" << QFileInfo(m_fileName).fileName();
	}
}

Plugin::~Plugin()
{
	delete m_translator;
}

void Plugin::init()
{
	if(!isInited()) {
		m_plugin->init();
		m_inited = true;
	}
}

void Plugin::load()
{
	if(!isLoaded()) {
		init();

		m_plugin->load();
		m_loaded = true;
	} else {
		qWarning() << "Trying to load already loaded plugin" << QFileInfo(m_fileName).fileName();
	}
}

void Plugin::unload()
{

}

void Plugin::changeLocale(const QString &locale)
{
	if(KittySDK::IPluginInfo *info = m_plugin->info()) {
		if(!info->id().isEmpty()) {
			if(!m_translator) {
				m_translator = new QTranslator();
			}

			QString dir = qApp->applicationDirPath() + "/data/translations/";
			if(m_translator->load(info->id() + "_" + locale, dir)) {
				QApplication::installTranslator(m_translator);
			}
		}
	}
}

const QList<Plugin*> &PluginManager::plugins() const
{
	return m_plugins;
}

Plugin *PluginManager::pluginByName(const QString &name) const
{
	foreach(Plugin *plugin, m_plugins) {
		if(plugin->isLoaded()) {
			if(plugin->iplugin() && plugin->iplugin()->info()) {
				if(plugin->iplugin()->info()->name() == name) {
					return plugin;
				}
			}
		}
	}

	return 0;
}

Plugin *PluginManager::pluginByFileName(const QString &fileName) const
{
	foreach(Plugin *plugin, m_plugins) {
		if(QFileInfo(plugin->fileName()).fileName() == fileName) {
			return plugin;
		}
	}

	return 0;
}

void PluginManager::execAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args)
{
	foreach(Plugin *plugin, m_plugins) {
		if(KittySDK::IPlugin *iplugin = plugin->iplugin()) {
			if(KittySDK::IPluginInfo *info = iplugin->info()) {
				if(pluginId.isEmpty() || (info->id() == pluginId)) {
					iplugin->execAction(name, args);
				}
			}
		}
	}
}

void PluginManager::load()
{
	qDebug() << "Starting";
	QDir dir(qApp->applicationDirPath() + "/plugins");
	QStringList filter;

#if defined(Q_OS_WIN)
	filter.append("*.dll");
#elif defined(Q_OS_MAC)
	filter.append("*.dylib");
	filter.append("*.so");
#elif defined(Q_OS_LINUX)
	filter.append("*.so");
#endif

	QFileInfoList files = dir.entryInfoList(filter, QDir::Files);
	foreach(const QFileInfo &info, files) {
		qDebug() << "Found file: " << info.fileName();

		Plugin *plug = new Plugin(info.absoluteFilePath());
		plug->load();
		m_plugins.append(plug);
	}

	emit allPluginsLoaded();
}

PluginManager::~PluginManager()
{
	qDeleteAll(m_plugins);
}

PluginManager::PluginManager(QObject *parent):
	QObject(parent)
{
	connect(this, SIGNAL(allPluginsLoaded()), SLOT(updateLanguages()));
	connect(Core::inst()->settingsWindow(), SIGNAL(settingsApplied()), SLOT(updateLanguages()));
}

void PluginManager::updateLanguages()
{
	QString locale = Core::inst()->setting(KittySDK::Settings::S_LANGUAGE, QLocale::system().name()).toString();
	foreach(Plugin *plugin, m_plugins) {
		plugin->changeLocale(locale);
	}
}

}
