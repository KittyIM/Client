#ifndef PLUGINCOREIMPL_H
#define PLUGINCOREIMPL_H

#include "SDK/PluginCore.h"

namespace KittySDK
{
  class SettingPage;
  class Account;
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
      void addAccount(KittySDK::Account *account);

      QPixmap icon(const QString &id);
      void addIcon(const QString &id, const QPixmap &pixmap, bool replace = true);
  };
}

#endif // PLUGINCOREIMPL_H
