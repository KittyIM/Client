#ifndef PLUGINCORE2_H
#define PLUGINCORE2_H

#include "SDK/PluginCore.h"

namespace Kitty
{
  class PluginCoreImpl: public KittySDK::PluginCore
  {
    Q_OBJECT

    public:
      QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());
      void setSetting(const QString &key, const QVariant &value);
  };
}

#endif // PLUGINCORE2_H
