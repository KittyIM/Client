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

              settings.remove("protocol");
              settings.remove("account");

              cnt->loadSettings(settings);
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
