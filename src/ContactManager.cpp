#include "ContactManager.h"

#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "Core.h"

#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[ContactManager]"
#define qWarning() qWarning() << "[ContactManager]"

namespace Kitty
{

ContactManager::~ContactManager()
{
	qDeleteAll(m_contacts);
}

KittySDK::IContact *ContactManager::contact(KittySDK::IAccount *acc, const QString &uid) const
{
	foreach(KittySDK::IContact *cnt, contactsByAccount(acc)) {
		if(cnt->uid() == uid) {
			return cnt;
		}
	}

	return 0;
}

const QList<KittySDK::IContact*> &ContactManager::contacts() const
{
	return m_contacts;
}

const QList<KittySDK::IContact*> ContactManager::contactsByProtocol(const QString &proto)
{
	QList<KittySDK::IContact*> contacts;

	foreach(KittySDK::IContact *cnt, m_contacts) {
		if(cnt->protocol()->protoInfo()->protoName() == proto) {
			contacts.append(cnt);
		}
	}

	return contacts;
}

const QList<KittySDK::IContact*> ContactManager::contactsByProtocol(KittySDK::IProtocol *proto)
{
	QList<KittySDK::IContact*> contacts;

	foreach(KittySDK::IContact *cnt, m_contacts) {
		if(cnt->protocol() == proto) {
			contacts.append(cnt);
		}
	}

	return contacts;
}

const QList<KittySDK::IContact*> ContactManager::contactsByAccount(const QString &acc, const QString &proto)
{
	QList<KittySDK::IContact*> contacts;

	foreach(KittySDK::IContact *cnt, m_contacts) {
		if((cnt->protocol()->protoInfo()->protoName() == proto) && (cnt->account()->uid() == acc)) {
			contacts.append(cnt);
		}
	}

	return contacts;
}

const QList<KittySDK::IContact *> ContactManager::contactsByAccount(KittySDK::IAccount *acc) const
{
	QList<KittySDK::IContact*> contacts;

	foreach(KittySDK::IContact *cnt, m_contacts) {
		if(cnt->account() == acc) {
			contacts.append(cnt);
		}
	}

	return contacts;
}

const QStringList ContactManager::groups() const
{
	QStringList list;

	foreach(KittySDK::IContact *cnt, m_contacts) {
		if(!cnt->group().isEmpty() && !list.contains(cnt->group())) {
			list.append(cnt->group());
		}
	}

	list.sort();

	return list;
}

void ContactManager::add(KittySDK::IContact *contact)
{
	m_contacts.append(contact);

	connect(contact, SIGNAL(statusChanged(KittySDK::IProtocol::Status,QString)), this, SIGNAL(statusUpdated()));

	emit contactAdded(contact);
}

void ContactManager::load(const QString &profile)
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
						KittySDK::IAccount *account = AccountManager::inst()->account(settings.value("protocol").toString(), settings.value("account").toString());
						if(account) {
							KittySDK::IContact *cnt = account->newContact(settings.value("uid").toString());
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

void ContactManager::save(const QString &profile)
{
	qDebug() << "saving contacts for" << profile;

	QVariantList list;

	foreach(KittySDK::IContact *contact, m_contacts) {
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

}
