#include "PluginManager.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "ProtocolManager.h"
#include "PluginCoreImpl.h"
#include "ChatManager.h"
#include "Core.h"

#include <ISettingsPage.h>
#include <SDKConstants.h>
#include <IProtocol.h>

#include <QtCore/QTranslator>
#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[PluginManager]"
#define qWarning() qWarning() << "[PluginManager]"

#ifdef QT_NO_DEBUG
	#define KITTY_DEBUG_STR "false"
#else
	#define KITTY_DEBUG_STR "true"
#endif

namespace Kitty
{

typedef QObject *(*pluginInst)(KittySDK::IPluginCore*);
typedef const char *(*pluginInfo)();

Plugin::Plugin(const QString &fileName):
	m_fileName(fileName),
	m_plugin(0),
	m_translator(0)
{
	m_loaded = false;
	m_inited = false;
}

Plugin::~Plugin()
{
	delete m_translator;
}

bool Plugin::setup(PluginCoreImpl *pcore)
{
	QLibrary lib(m_fileName);

	if(pluginInfo info = (pluginInfo)lib.resolve("info")) {
		QStringList lines = QString(info()).split("\n");
		if(lines.count() == 3) {
			if(lines[0] != QString("sdkversion=" KITTYSDK_VERSION)) {
				QString sdkver = lines[0].split('=').at(1);
				m_error = QObject::tr("Wrong SDK version \"%1\", Kitty has '\"%2\".").arg(sdkver).arg(KITTYSDK_VERSION);

				lib.unload();
				return false;
			}

			if(lines[1] != QString("debug=" KITTY_DEBUG_STR)) {
				QString mode = lines[1].split('=').at(1);
				m_error = QObject::tr("Wrong build mode \"%1\", Kitty is \"%2\".").arg((mode == "true") ? "debug" : "release").arg((QString(KITTY_DEBUG_STR) == "true") ? "debug" : "release");

				lib.unload();
				return false;
			}

			if(lines[2] != QString("buildkey=" QT_BUILD_KEY)) {
				QString buildkey = lines[2].split('=').at(1);
				m_error = QObject::tr("Wrong Qt build key \"%1\", Kitty has \"%2\".").arg(buildkey).arg(QT_BUILD_KEY);

				lib.unload();
				return false;
			}
		} else {
			m_error = QObject::tr("Wrong verification info count.");

			lib.unload();
			return false;
		}

		pluginInst inst = (pluginInst)lib.resolve("inst");
		if(!inst) {
			m_error = QObject::tr("Can't resolve info().");

			lib.unload();
			return false;
		}

		m_plugin = dynamic_cast<KittySDK::IPlugin*>(inst(pcore));
		if(!m_plugin) {
			m_error = QObject::tr("Could not cast to IPlugin.");

			lib.unload();
			return false;
		}

		if(m_plugin->type() == KittySDK::IProtocol::Type) {
			KittySDK::IProtocol *proto = dynamic_cast<KittySDK::IProtocol*>(m_plugin);
			if(!proto) {
				m_error = QObject::tr("Could not cast to IProtocol.");

				lib.unload();
				return false;
			}

			Core::inst()->protocolManager()->add(proto);
		} else if(m_plugin->type() == KittySDK::IPlugin::Type) {
			//nothing for now
		} else {
			m_error = QObject::tr("Invalid plugin type.");

			lib.unload();
			return false;
		}
	}

	return true;
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
	if(isLoaded()) {
		m_plugin->unload();
		m_loaded = false;
	}
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

bool Plugin::hasError() const
{
	return !m_error.isEmpty();
}

const QList<Plugin*> &PluginManager::plugins() const
{
	return m_plugins;
}


PluginManager::PluginManager(Core *core):
	QObject(core),
	m_core(core),
	m_pluginCore(0)
{
	connect(this, SIGNAL(allLoaded()), SLOT(updateLanguages()));
	connect(m_core, SIGNAL(languageChanged()), SLOT(updateLanguages()));
}

void PluginManager::updateLanguages()
{
	QString locale = m_core->setting(KittySDK::Settings::S_LANGUAGE).toString();
	if(locale.isEmpty()) {
		locale = QLocale::system().name();
	}

	foreach(Plugin *plugin, m_plugins) {
		if(plugin->hasError()) {
			continue;
		}

		plugin->changeLocale(locale);
		plugin->iplugin()->execAction("retranslate", QMap<QString, QVariant>());
	}
}

PluginManager::~PluginManager()
{
	qDeleteAll(m_plugins);
}


Plugin *PluginManager::pluginById(const QString &id) const
{
	foreach(Plugin *plugin, m_plugins) {
		if(plugin->isLoaded()) {
			if(plugin->iplugin() && plugin->iplugin()->info()) {
				if(plugin->iplugin()->info()->id() == id) {
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

const QMap<KittySDK::ISettingsPage *, QString> &PluginManager::settingsPages() const
{
	return m_settingsPages;
}

void PluginManager::addSettingsPage(KittySDK::ISettingsPage *page, const QString &parent)
{
	m_settingsPages.insert(page, parent);

	page->setProperty("plugin", true);

	emit settingsPageAdded(page, parent);
}

bool PluginManager::hasError() const
{
	foreach(Plugin *plug, m_plugins) {
		if(plug->hasError()) {
			return true;
		}
	}

	return false;
}

void PluginManager::execAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args)
{
	if(pluginId == "core") {
		if(name == "openChat") {
			QString chatId = args.value("chatId").toString();
			if(!chatId.isEmpty()) {
				if(KittySDK::IChat *chat = m_core->chatManager()->chat(chatId)) {
					m_core->chatWindow()->showChat(chat);
				}
			}
		}
	} else {
		foreach(Plugin *plugin, m_plugins) {
			if(plugin->hasError()) {
				continue;
			}

			if(KittySDK::IPlugin *iplugin = plugin->iplugin()) {
				if(KittySDK::IPluginInfo *info = iplugin->info()) {
					if(pluginId.isEmpty() || (info->id() == pluginId)) {
						iplugin->execAction(name, args);
					}
				}
			}
		}
	}
}

void PluginManager::load()
{
	//qDebug() << "Starting";
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

	if(!m_pluginCore) {
		m_pluginCore = new PluginCoreImpl(m_core);
	}

	QFileInfoList files = dir.entryInfoList(filter, QDir::Files);
	foreach(const QFileInfo &info, files) {
		Plugin *plug = new Plugin(info.absoluteFilePath());
		if(plug->setup(m_pluginCore)) {
			plug->load();
		}
		m_plugins.append(plug);
	}

	emit allLoaded();
}

}
