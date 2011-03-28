#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "SDK/Plugin.h"
#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
  class PluginManager: public QObject, public Singleton<PluginManager>
  {
    Q_OBJECT

    friend class Singleton<PluginManager>;

    public:
      void load();

    private:
      explicit PluginManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<KittySDK::Plugin*> m_plugins;
  };
}

#endif // PLUGINMANAGER_H
