#include "AccountManager.h"

#include <QtCore/QDebug>

const QList<KittySDK::Account*> &Kitty::AccountManager::accounts() const
{
  return m_accounts;
}

const QList<KittySDK::Account*> Kitty::AccountManager::accountsByProtocol(KittySDK::Protocol *protocol) const
{
  QList<KittySDK::Account*> accounts;

  foreach(KittySDK::Account *account, m_accounts) {
    if(account->protocol() == protocol) {
      accounts.append(account);
    }
  }

  return accounts;
}

KittySDK::Account *Kitty::AccountManager::account(KittySDK::Protocol *protocol, const QString &uid) const
{
  foreach(KittySDK::Account *account, accountsByProtocol(protocol)) {
    if(account->uid() == uid) {
      return account;
    }
  }

  return 0;
}

void Kitty::AccountManager::add(KittySDK::Account *account)
{
  qDebug() << "Adding new account" << account->uid() << account->password() << account->protocol()->protoInfo()->protoName();
  m_accounts.append(account);
}


