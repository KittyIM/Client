#include "AccountManager.h"

#include "widgets/windows/MainWindow.h"
#include "3rdparty/json/json.h"
#include "ProtocolManager.h"
#include "ContactManager.h"
#include "PluginManager.h"
#include "SDK/constants.h"
#include "ChatManager.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QAction>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[AccountManager]"
#define qWarning() qWarning() << "[AccountManager]"

using namespace Kitty;
using namespace KittySDK;

Kitty::AccountManager::~AccountManager()
{
  qDeleteAll(m_accounts);
}

const QList<Account*> &Kitty::AccountManager::accounts() const
{
  return m_accounts;
}

const QList<Account*> Kitty::AccountManager::accountsByProtocol(Protocol *protocol) const
{
  QList<Account*> accounts;

  foreach(Account *account, m_accounts) {
    if(account->protocol() == protocol) {
      accounts.append(account);
    }
  }

  return accounts;
}

Account *Kitty::AccountManager::account(Protocol *protocol, const QString &uid) const
{
  foreach(Account *account, accountsByProtocol(protocol)) {
    if(account->uid() == uid) {
      return account;
    }
  }

  return 0;
}

Account *Kitty::AccountManager::account(const QString &protocol, const QString &uid) const
{
  Protocol *proto = ProtocolManager::inst()->protocolByName(protocol);
  if(proto) {
    foreach(Account *account, accountsByProtocol(proto)) {
      if(account->uid() == uid) {
        return account;
      }
    }
  }

  return 0;
}

bool Kitty::AccountManager::add(Account *account)
{
  qDebug() << "Adding new account" << account->uid() << account->password() << account->protocol()->protoInfo()->protoName();

  foreach(Account *acc, accountsByProtocol(account->protocol())) {
    if(account->uid() == acc->uid()) {
      return false;
    }
  }

  connect(account, SIGNAL(messageReceived(KittySDK::Message&)), ChatManager::inst(), SLOT(receiveMessage(KittySDK::Message&)));
  connect(account, SIGNAL(contactAdded(KittySDK::Contact*)), ContactManager::inst(), SLOT(add(KittySDK::Contact*)));

  account->loadSettings(QMap<QString, QVariant>());

  if(account->protocol()->abilities().testFlag(Protocol::ChangeStatus)) {
    QAction *action = new QAction(this);
    action->setText(QString("%1 (%2)").arg(account->uid()).arg(account->protocol()->protoInfo()->protoName()));
    action->setIcon(Core::inst()->icon(account->protocol()->statusIcon(account->status())));
    action->setProperty("protocol", account->protocol()->protoInfo()->protoName());
    action->setProperty("uid", account->uid());
    connect(account, SIGNAL(statusChanged(KittySDK::Protocol::Status, QString)), Core::inst()->mainWindow(), SLOT(updateAccountStatusIcon()));
    connect(action, SIGNAL(triggered()), Core::inst()->mainWindow(), SLOT(showAccountStatusMenu()));

    Core::inst()->mainWindow()->addToolbarAction(Toolbars::TB_NETWORKS, action);
  }

  m_accounts.append(account);

  emit accountAdded();

  return true;
}

void Kitty::AccountManager::load(const QString &profile)
{
  qDebug() << "Loading accounts for" << profile;

  QFile file(Core::inst()->profilesDir() + profile + "/accounts.dat");
  if(file.exists()) {
    if(file.open(QIODevice::ReadOnly)) {
      QVariantMap map = Json::parse(qUncompress(file.readAll())).toMap();
      if(map.contains("accounts")) {
        QVariantList list = map.value("accounts").toList();
        foreach(QVariant item, list) {
          QVariantMap settings = item.toMap();

          if(settings.contains("protocol")) {
            Protocol *proto = ProtocolManager::inst()->protocolByName(settings.value("protocol").toString());

            if(proto) {
              Plugin *plug = PluginManager::inst()->pluginByName(proto->info()->name());
              if(plug->isLoaded()) {
                Account *acc = proto->newAccount(settings.value("uid").toString());
                if(acc) {
                  acc->setPassword(settings.value("password").toString());

                  settings.remove("protocol");
                  settings.remove("uid");
                  settings.remove("password");

                  acc->loadSettings(settings);

                  add(acc);
                } else {
                  qWarning() << "Account creation failed" << settings.value("protocol").toString() << settings.value("uid").toString();
                }
              }
            } else {
              qWarning() << "Protocole not found" << settings.value("protocol").toString();
            }
          } else {
            qWarning() << "Account #" << list.indexOf(item) << " had no protocol info!";
          }
        }
      } else {
        qDebug() << "No accounts info!";
      }

      file.close();
    } else {
      qDebug() << "Could not open file!";
    }
  }
}

void Kitty::AccountManager::save(const QString &profile)
{
  qDebug() << "saving accounts for" << profile;

  QVariantList list;
  foreach(Account *account, m_accounts) {
    QVariantMap acc;

    QMap<QString, QVariant> settings = account->saveSettings();
    settings.insert("protocol", account->protocol()->protoInfo()->protoName());
    settings.insert("uid", account->uid());
    settings.insert("password", account->password());

    QMapIterator<QString, QVariant> i(settings);
    while(i.hasNext()) {
      i.next();

      acc.insert(i.key(), i.value());
    }

    list.append(acc);
  }

  QVariantMap map;
  map.insert("accounts", list);

  QFile file(Core::inst()->profilesDir() + profile + "/accounts.dat");
  if(file.open(QIODevice::ReadWrite)) {
    file.resize(0);
    file.write(qCompress(Json::stringify(map).toAscii()));
    file.close();
  }
}

void Kitty::AccountManager::changeDescription(const QString &description)
{
  foreach(Account *acc, m_accounts) {
    acc->changeStatus(acc->status(), description);
  }
}

void Kitty::AccountManager::changeStatus(KittySDK::Protocol::Status status)
{
  foreach(Account *acc, m_accounts) {
    acc->changeStatus(status, acc->description());
  }
}
