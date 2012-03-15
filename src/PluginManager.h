#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <IPlugin.h>

#include <QtCore/QDebug>

class QTranslator;

namespace Kitty
{
	class Core;

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
			bool hasError() const;

			KittySDK::IPlugin *iplugin() const { return m_plugin; }
			QString fileName() const { return m_fileName; }
			QString error() const { return m_error; }
			bool isLoaded() const { return m_loaded; }
			bool isInited() const { return m_inited; }

		private:
			bool m_loaded;
			bool m_inited;
			QString m_fileName;
			QString m_error;
			KittySDK::IPlugin *m_plugin;
			QTranslator *m_translator;
	};

	class PluginManager: public QObject
	{
		Q_OBJECT

		public:
			PluginManager(Core *core);
			~PluginManager();

			const QList<Plugin*> &plugins() const;
			Plugin *pluginById(const QString &id) const;
			Plugin *pluginByFileName(const QString &fileName) const;

			bool hasError() const;

			void execAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args);

			void load();

		signals:
			void allLoaded();

		private slots:
			void updateLanguages();

		private:
			QList<Plugin*> m_plugins;
			Core *m_core;
	};
}

#endif // PLUGINMANAGER_H
