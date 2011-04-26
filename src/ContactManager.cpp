#include "ContactManager.h"

#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "Core.h"

#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

const QList<KittySDK::Contact*> &Kitty::ContactManager::contacts() const
{
  return m_contacts;
}

void Kitty::ContactManager::add(KittySDK::Contact *contact)
{
  m_contacts.append(contact);
}

void Kitty::ContactManager::load(const QString &profile)
{
  qDebug() << "Loading contacts for" << profile;

  QFile file(Kitty::Core::inst()->profilesDir() + profile + "/contacts.json");
  if(file.exists()) {
    if(file.open(QIODevice::ReadOnly)) {
      QVariantMap map = Json::parse(file.readAll()).toMap();
      if(map.contains("contacts")) {
        QVariantList list = map.value("contacts").toList();
        foreach(QVariant item, list) {
          QVariantMap settings = item.toMap();

          if(settings.contains("protocol") && settings.contains("account")) {
            KittySDK::Account *account = Kitty::AccountManager::inst()->account(settings.value("protocol").toString(), settings.value("account").toString());
            if(account) {
              KittySDK::Contact *cnt = account->newContact(settings.value("uid").toString());
              cnt->setDisplay(settings.value("display").toString());
              cnt->setGroup(settings.value("group").toString());

              connect(cnt, SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SIGNAL(statusUpdated()));

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

  foreach(KittySDK::Contact *contact, m_contacts) {
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

  QFile file(Core::inst()->profilesDir() + profile + "/contacts.json");
  if(file.open(QIODevice::ReadWrite)) {
    file.resize(0);

    QTextStream str(&file);
    str.setCodec("UTF-8");
    str << Json::stringify(map);

    file.close();
  }
}
