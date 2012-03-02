#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "Singleton.h"

#include <IPlugin.h>

#include <QtCore/QDebug>

class QTranslator;

namespace Kitty
{
	class Plugin
	{
		public:
			Plugin(const QString &fileName);
			~Plugin();

			bool setup();
			void init();
			void load();
			void unload();
			void changeLocale(const QString &locale);

			KittySDK::IPlugin *iplugin() const { return m_plugin; }
			QString fileName() const { return m_fileName; }
			bool isLoaded() const { return m_loaded; }
			bool isInited() const { return m_inited; }

		private:
			bool m_loaded;
			bool m_inited;
			QString m_fileName;
			KittySDK::IPlugin *m_plugin;
			QTranslator *m_translator;
	};

	class PluginManager: public QObject, public Singleton<PluginManager>
	{
		Q_OBJECT

		friend class Singleton<PluginManager>;

		public:
			const QList<Plugin*> &plugins() const;
			Plugin *pluginById(const QString &id) const;
			Plugin *pluginByFileName(const QString &fileName) const;

			void execAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args);

			void load();

		signals:
			void allPluginsLoaded();

		private slots:
			void updateLanguages();

		private:
			explicit PluginManager(QObject *parent = 0);
			~PluginManager();

		private:
			QList<Plugin*> m_plugins;
	};
}

#endif // PLUGINMANAGER_H
