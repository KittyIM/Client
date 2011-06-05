#ifndef PLUGINCOREIMPL_H
#define PLUGINCOREIMPL_H

#include "SDK/PluginCore.h"

namespace KittySDK
{
  class SettingPage;
  class Contact;
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
      QString avatarPath(KittySDK::Contact *contact);

      int contactCount();
      KittySDK::Contact *contact(const int &id);
      QList<KittySDK::Contact*> contacts(const QString &account, const QString &protocol);
      QList<KittySDK::Contact*> contacts(const QString &protocol);

      QStringList plugins();

      QVariant jsonParse(const QString &json);
      QString jsonStringify(const QVariant &json, int indent);

      void addSettingPage(KittySDK::SettingPage *page, const QString &parent);
      void removeSettingPage(KittySDK::SettingPage *page);

      void addToolbarAction(const QString &tb, QAction *action);
      void removeToolbarAction(const QString &tb, QAction *action);
      QToolButton *buttonForAction(const QString &tb, QAction *action);

      void addAccount(KittySDK::Account *account);

      QPixmap icon(const QString &id);
      void addIcon(const QString &id, const QPixmap &pixmap, bool replace = true);
      void removeIcon(const QString &id);
  };
}

#endif // PLUGINCOREIMPL_H
