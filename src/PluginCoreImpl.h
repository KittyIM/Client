#ifndef PLUGINCORE2_H
#define PLUGINCORE2_H

#include "SDK/PluginCore.h"

namespace KittySDK
{
  class SettingPage;
}

namespace Kitty
{
  class PluginCoreImpl: public KittySDK::PluginCore
  {
    Q_OBJECT

    public:
      QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());
      void setSetting(const QString &key, const QVariant &value);

      void addSettingPage(KittySDK::SettingPage *page, const QString &parent);

      QPixmap icon(const QString &id);
      void addIcon(const QString &id, const QPixmap &pixmap, bool replace = true);
  };
}

#endif // PLUGINCORE2_H
