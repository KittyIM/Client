#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

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
      explicit PluginManager(QObject *parent = 0);

  };
}

#endif // PLUGINMANAGER_H
