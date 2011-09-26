#include "ContactManager.h"

#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "Core.h"

#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[ContactManager]"
#define qWarning() qWarning() << "[ContactManager]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ContactManager::~ContactManager()
{
  qDeleteAll(m_contacts);
}

const QList<Contact*> &Kitty::ContactManager::contacts() const
{
  return m_contacts;
}

const QList<Contact*> Kitty::ContactManager::contactsByProtocol(const QString &proto)
{
  QList<Contact*> contacts;

  foreach(Contact *cnt, m_contacts) {
    if(cnt->protocol()->protoInfo()->protoName() == proto) {
      contacts.append(cnt);
    }
  }

  return contacts;
}

const QList<Contact*> Kitty::ContactManager::contactsByProtocol(Protocol *proto)
{
  QList<Contact*> contacts;

  foreach(Contact *cnt, m_contacts) {
    if(cnt->protocol() == proto) {
      contacts.append(cnt);
    }
  }

  return contacts;
}

const QList<Contact*> Kitty::ContactManager::contactsByAccount(const QString &acc, const QString &proto)
{
  QList<Contact*> contacts;

  foreach(Contact *cnt, m_contacts) {
    if((cnt->protocol()->protoInfo()->protoName() == proto) && (cnt->account()->uid() == acc)) {
      contacts.append(cnt);
    }
  }

  return contacts;
}

const QStringList Kitty::ContactManager::groups() const
{
  QStringList list;

  foreach(Contact *cnt, m_contacts) {
    if(!cnt->group().isEmpty() && !list.contains(cnt->group())) {
      list.append(cnt->group());
    }
  }

  list.sort();

  return list;
}

void Kitty::ContactManager::add(Contact *contact)
{
  m_contacts.append(contact);

  connect(contact, SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SIGNAL(statusUpdated()));

  emit contactAdded(contact);
}

void Kitty::ContactManager::load(const QString &profile)
{
  qDebug() << "Loading contacts for" << profile;

  QFile file(Core::inst()->profilesDir() + profile + "/contacts.dat");
  if(file.exists()) {
    if(file.open(QIODevice::ReadOnly)) {
      QVariantMap map = Json::parse(qUncompress(file.readAll())).toMap();
      if(map.contains("contacts")) {
        QVariantList list = map.value("contacts").toList();
        foreach(QVariant item, list) {
          QVariantMap settings = item.toMap();

          if(settings.contains("protocol") && settings.contains("account")) {
            Account *account = AccountManager::inst()->account(settings.value("protocol").toString(), settings.value("account").toString());
            if(account) {
              Contact *cnt = account->newContact(settings.value("uid").toString());
              cnt->setDisplay(settings.value("display").toString());
              cnt->setGroup(settings.value("group").toString());

              settings.remove("protocol");
              settings.remove("account");
              settings.remove("group");

              cnt->loadSettings(settings);

              account->insertContact(cnt->uid(), cnt);
              add(cnt);
            }
          } else {
            qWarning() << "No protocol and/or account info for contact #" << list.indexOf(item);
          }
        }
      }

      file.close();
    } else {
      qWarning() << "Could not open file!";
    }
  }
}

void Kitty::ContactManager::save(const QString &profile)
{
  qDebug() << "saving contacts for" << profile;

  QVariantList list;

  foreach(Contact *contact, m_contacts) {
    QMap<QString, QVariant> settings = contact->saveSettings();
    settings.insert("protocol", contact->account()->protocol()->protoInfo()->protoName());
    settings.insert("account", contact->account()->uid());
    settings.insert("uid", contact->uid());
    settings.insert("display", contact->display());
    settings.insert("group", contact->group());

    list.append(settings);
  }

  QVariantMap map;
  map.insert("contacts", list);

  QFile file(Core::inst()->profilesDir() + profile + "/contacts.dat");
  if(file.open(QIODevice::ReadWrite)) {
    file.resize(0);
    file.write(qCompress(Json::stringify(map).toAscii()));
    file.close();
  }
}
