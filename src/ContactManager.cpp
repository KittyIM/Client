#include "ContactManager.h"

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

  QFile file(Kitty::Core::inst()->profilesDir() + profile + "/contacts.xml");
  if(file.exists()) {
    if(file.open(QIODevice::ReadOnly)) {
      QDomDocument doc;
      doc.setContent(&file);

      QDomElement root = doc.documentElement();
      QDomNodeList groups = root.elementsByTagName("group");
      for(int i = 0; i < groups.count(); i++) {
        QDomNode group = groups.at(i);
        QString groupName = group.toElement().attribute("name");

        QDomNodeList contacts = group.toElement().elementsByTagName("contact");
        for(int j = 0; j < contacts.count(); j++) {
          QDomNode contact = contacts.at(j);
          QDomNodeList children = contact.childNodes();
          QMap<QString, QVariant> settings;

          for(int k = 0; k < children.count(); k++) {
            settings.insert(children.at(k).nodeName(), children.at(k).firstChild().nodeValue());
          }

          if(settings.contains("protocol") && settings.contains("account")) {
            KittySDK::Account *account = Kitty::AccountManager::inst()->account(settings.value("protocol").toString(), settings.value("account").toString());
            if(account) {
              KittySDK::Contact *cnt = account->newContact(settings.value("uid").toString());
              cnt->setDisplay(settings.value("display").toString());
              cnt->setGroup(groupName);

              connect(cnt, SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SIGNAL(statusUpdated()));

              settings.remove("protocol");
              settings.remove("account");

              cnt->loadSettings(settings);

              account->insertContact(cnt->uid(), cnt);
              add(cnt);
            }
          } else {
            qWarning() << "No protocol and/or account info for contact in group" << groupName;
          }
        }
      }

      file.close();
    } else {
      qDebug() << "Could not open accounts file for" << profile;
    }
  }
}

void Kitty::ContactManager::save(const QString &profile)
{
  qDebug() << "saving contacts for" << profile;

  QDomDocument doc;
  QDomElement root = doc.createElement("contacts");

  QMap<QString, QDomElement> groups;
  foreach(KittySDK::Contact *contact, m_contacts) {
    if(!contact->group().isEmpty() && !groups.contains(contact->group())) {
      QDomElement elem = doc.createElement("group");
      elem.setAttribute("name", contact->group());
      groups.insert(contact->group(), elem);
    }
  }

  foreach(KittySDK::Contact *contact, m_contacts) {
    QDomElement cnt = doc.createElement("contact");

    QMap<QString, QVariant> settings = contact->saveSettings();
    settings.insert("protocol", contact->account()->protocol()->protoInfo()->protoName());
    settings.insert("account", contact->account()->uid());
    settings.insert("uid", contact->uid());
    settings.insert("display", contact->display());

    QMapIterator<QString, QVariant> i(settings);
    while(i.hasNext()) {
      i.next();

      QDomElement elem = doc.createElement(i.key());
      elem.appendChild(doc.createTextNode(i.value().toString()));
      cnt.appendChild(elem);
    }

    if(contact->group().isEmpty()) {
      root.appendChild(cnt);
    } else {
      QDomElement group = groups.value(contact->group());
      group.appendChild(cnt);
    }
  }

  QMapIterator<QString, QDomElement> ig(groups);
  while(ig.hasNext()) {
    ig.next();

    root.appendChild(ig.value());
  }

  doc.appendChild(root);

  QFile file(Core::inst()->profilesDir() + profile + "/contacts.xml");
  if(file.open(QIODevice::ReadWrite)) {
    file.resize(0);

    QTextStream str(&file);
    str.setCodec("UTF-8");
    str << doc.toString(2);

    file.close();
  }
}
