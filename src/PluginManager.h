#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <IPlugin.h>

#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
	class Plugin
	{
		public:
			Plugin(const QString &fileName);

			void init();
			void load();
			void unload();

			KittySDK::IPlugin *plugin() const { return m_plugin; }
			QString fileName() const { return m_fileName; }
			bool isLoaded() const { return m_loaded; }
			bool isInited() const { return m_inited; }

		private:
			bool m_loaded;
			bool m_inited;
			QString m_fileName;
			KittySDK::IPlugin *m_plugin;
	};

	class PluginManager: public QObject, public Singleton<PluginManager>
	{
		Q_OBJECT

		friend class Singleton<PluginManager>;

		public:
			const QList<Plugin*> &plugins() const;
			Plugin *pluginByName(const QString &name) const;
			Plugin *pluginByFileName(const QString &fileName) const;

			void execAction(const QString &plugin, const QString &name, const QMap<QString, QVariant> &args);

			void load();

		signals:
			void allPluginsLoaded();

		private:
			explicit PluginManager(QObject *parent = 0): QObject(parent) { }
			~PluginManager();

		private:
			QList<Plugin*> m_plugins;
	};
}

#endif // PLUGINMANAGER_H
