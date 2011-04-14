#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "SDK/Plugin.h"
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

      KittySDK::Plugin *plugin() const { return m_plugin; }
      QString fileName() const { return m_fileName; }
      bool isLoaded() const { return m_loaded; }
      bool isInited() const { return m_inited; }

    private:
      bool m_loaded;
      bool m_inited;
      QString m_fileName;
      KittySDK::Plugin *m_plugin;
  };

  class PluginManager: public QObject, public Singleton<PluginManager>
  {
    Q_OBJECT

    friend class Singleton<PluginManager>;

    public:
      const QList<Plugin*> &plugins() const;
      Plugin *pluginByName(const QString &name) const;
      Plugin *pluginByFileName(const QString &fileName) const;

      void load();

    private:
      explicit PluginManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<Plugin*> m_plugins;
  };
}

#endif // PLUGINMANAGER_H
