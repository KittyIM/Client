#include "AccountManager.h"

#include "widgets/windows/MainWindow.h"
#include "ProtocolManager.h"
#include "PluginManager.h"
#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QAction>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

using namespace Kitty;

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

KittySDK::Account *Kitty::AccountManager::account(const QString &protocol, const QString &uid) const
{
  KittySDK::Protocol *proto = ProtocolManager::inst()->protocolByName(protocol);
  if(proto) {
    foreach(KittySDK::Account *account, accountsByProtocol(proto)) {
      if(account->uid() == uid) {
        return account;
      }
    }
  }

  return 0;
}

bool Kitty::AccountManager::add(KittySDK::Account *account)
{
  qDebug() << "Adding new account" << account->uid() << account->password() << account->protocol()->protoInfo()->protoName();

  foreach(KittySDK::Account *acc, accountsByProtocol(account->protocol())) {
    if(account->uid() == acc->uid()) {
      return false;
    }
  }

  if(account->protocol()->abilities().testFlag(KittySDK::Protocol::ChangeStatus)) {
    QAction *action = new QAction(this);
    action->setText(QString("%1 (%2)").arg(account->uid()).arg(account->protocol()->protoInfo()->protoName()));
    action->setIcon(Core::inst()->icon(account->protocol()->statusIcon(account->status())));
    action->setProperty("protocol", account->protocol()->protoInfo()->protoName());
    action->setProperty("uid", account->uid());
    connect(account, SIGNAL(statusChanged()), Core::inst()->mainWindow(), SLOT(updateAccountStatusIcon()));
    connect(action, SIGNAL(triggered()), Core::inst()->mainWindow(), SLOT(showAccountStatusMenu()));

    Core::inst()->mainWindow()->addToolbarAction(KittySDK::Toolbars::TB_NETWORKS, action);
  }

  m_accounts.append(account);

  emit accountAdded();

  return true;
}

void Kitty::AccountManager::load(const QString &profile)
{
  qDebug() << "Loading accounts for" << profile;

  QFile file(Core::inst()->profilesDir() + profile + "/accounts.xml");
  if(file.exists()) {
    if(file.open(QIODevice::ReadOnly)) {
      QDomDocument doc;
      doc.setContent(&file);

      QDomElement root = doc.documentElement();
      QDomNodeList accounts = root.elementsByTagName("account");
      for(int i = 0; i < accounts.count(); i++) {
        QDomNodeList children = accounts.at(i).childNodes();
        QMap<QString, QVariant> settings;

        for(int j = 0; j < children.count(); j++) {
          settings.insert(children.at(j).nodeName(), children.at(j).firstChild().nodeValue());
        }

        if(settings.contains("protocol")) {
          KittySDK::Protocol *proto = ProtocolManager::inst()->protocolByName(settings.value("protocol").toString());

          if(proto) {
            Plugin *plug = PluginManager::inst()->pluginByName(proto->info()->name());
            if(plug->isLoaded()) {
              KittySDK::Account *acc = proto->newAccount(settings.value("uid").toString());
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
          qWarning() << "Account #" << i << " had no protocol info!";
        }
      }

      file.close();
    } else {
      qDebug() << "Could not open accounts file for" << profile;
    }
  }
}

void Kitty::AccountManager::save(const QString &profile)
{
  qDebug() << "saving accounts for" << profile;

  QDomDocument doc;
  QDomElement root = doc.createElement("accounts");

  foreach(KittySDK::Account *account, m_accounts) {
    QDomElement acc = doc.createElement("account");

    QMap<QString, QVariant> settings = account->saveSettings();
    settings.insert("protocol", account->protocol()->protoInfo()->protoName());
    settings.insert("uid", account->uid());
    settings.insert("password", account->password());

    QMapIterator<QString, QVariant> i(settings);
    while(i.hasNext()) {
      i.next();

      QDomElement elem = doc.createElement(i.key());
      elem.appendChild(doc.createTextNode(i.value().toString()));
      acc.appendChild(elem);
    }

    root.appendChild(acc);
  }

  doc.appendChild(root);

  QFile file(Core::inst()->profilesDir() + profile + "/accounts.xml");
  if(file.open(QIODevice::ReadWrite)) {
    file.resize(0);

    QTextStream str(&file);
    str.setCodec("UTF-8");
    str << doc.toString(2);

    file.close();
  }
}
