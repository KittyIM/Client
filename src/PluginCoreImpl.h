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
      PluginCoreImpl(QObject *parent = 0);

      QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());
      void setSetting(const QString &key, const QVariant &value);

      void execPluginAction(const QString &plugin, const QString &name, const QMap<QString, QVariant> &args);

      QString profileName();
      QString kittyDir();
      QString profilesDir();

      void addSettingPage(KittySDK::SettingPage *page, const QString &parent);
      void addToolbarAction(const QString &tb, QAction *action);
      void addAccount(KittySDK::Account *account);

      QPixmap icon(const QString &id);
      void addIcon(const QString &id, const QPixmap &pixmap, bool replace = true);
  };
}

#endif // PLUGINCOREIMPL_H
