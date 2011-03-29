#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "SDK/Account.h"
#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
  class AccountManager: public QObject, public Singleton<AccountManager>
  {
    Q_OBJECT

    friend class Singleton<AccountManager>;

    public:
      const QList<KittySDK::Account*> &accounts() const;
      const QList<KittySDK::Account*> accountsByProtocol(KittySDK::Protocol *protocol) const;

      KittySDK::Account *account(KittySDK::Protocol *protocol, const QString &uid) const;

      void add(KittySDK::Account *account);

    private:
      AccountManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<KittySDK::Account*> m_accounts;
  };
}

#endif // ACCOUNTMANAGER_H
