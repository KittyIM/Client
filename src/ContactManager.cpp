#include "ContactManager.h"

#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "PluginManager.h"
#include "Core.h"

#include <SoundsConstants.h>

#include <QtCore/QFile>
#include <QtGui/QTextDocument>
#include <QtGui/QPixmap>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[ContactManager]"
#define qWarning() qWarning() << "[ContactManager]"

namespace Kitty
{

ContactManager::ContactManager(Core *core):
	QObject(core),
	m_core(core)
{
}

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
	if(!m_contacts.contains(contact)) {
		m_contacts.append(contact);

		connect(contact, SIGNAL(statusChanged(KittySDK::IProtocol::Status,QString)), SLOT(updateStatus(KittySDK::IProtocol::Status,QString)));

		emit contactAdded(contact);
	}
}

void ContactManager::load(const QString &profile)
{
	//qDebug() << "Loading contacts for" << profile;

	QFile file(Core::inst()->profilesDir() + profile + "/contacts.dat");
	if(file.exists()) {
		if(file.open(QIODevice::ReadOnly)) {
			QVariantMap map = Json::parse(qUncompress(file.readAll())).toMap();
			if(map.contains("contacts")) {
				QVariantList list = map.value("contacts").toList();
				foreach(QVariant item, list) {
					QVariantMap settings = item.toMap();

					if(settings.contains("protocol") && settings.contains("account")) {
						KittySDK::IAccount *account = Core::inst()->accountManager()->account(settings.value("protocol").toString(), settings.value("account").toString());
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

	emit allLoaded();
}

void ContactManager::save(const QString &profile)
{
	//qDebug() << "saving contacts for" << profile;

	emit aboutToSave();

	QVariantList list;

	foreach(KittySDK::IContact *contact, m_contacts) {
		if(!contact->data(KittySDK::ContactInfos::I_TEMPORARY).toBool()) {
			QMap<QString, QVariant> settings = contact->saveSettings();
			settings.insert("protocol", contact->account()->protocol()->protoInfo()->protoName());
			settings.insert("account", contact->account()->uid());
			settings.insert("uid", contact->uid());
			settings.insert("display", contact->display());
			settings.insert("group", contact->group());

			list.append(settings);
		}
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

void ContactManager::updateStatus(KittySDK::IProtocol::Status status, const QString &description)
{
	if(KittySDK::IContact *cnt = static_cast<KittySDK::IContact*>(sender())) {
		bool status_changed = (cnt->status() != status);
		bool desc_changed = (cnt->description() != description);

		if((cnt->status() == KittySDK::IProtocol::Offline) && (status < KittySDK::IProtocol::Offline)) {
			QMap<QString, QVariant> soundsArgs;
			soundsArgs.insert("id", Sounds::Sounds::S_CONTACT_AVAIL);
			PluginManager::inst()->execAction("sounds", "playSound", soundsArgs);
		} else if((cnt->status() < KittySDK::IProtocol::Offline) && (status == KittySDK::IProtocol::Offline)) {
			QMap<QString, QVariant> soundsArgs;
			soundsArgs.insert("id", Sounds::Sounds::S_CONTACT_UNAVAIL);
			PluginManager::inst()->execAction("sounds", "playSound", soundsArgs);
		}

		if(status_changed || desc_changed) {
			QString notifyText = "<span class=\"notifyText\"><b>" + Qt::escape(cnt->display()) + "</b> ";

			bool female = false;
			if(cnt->data(KittySDK::ContactInfos::I_SEX).toInt() == 1) {
				female = true;
			} else if(cnt->data(KittySDK::ContactInfos::I_SEX).toInt() == 0) {
				QString firstname = cnt->data(KittySDK::ContactInfos::I_FIRSTNAME).toString();
				if(firstname.isEmpty()) {
					firstname = cnt->display();
				}

				//this rule works only for Polish language, sorry
				if((Core::inst()->setting(KittySDK::Settings::S_LANGUAGE).toString() == "pl") && (firstname.right(1) == "a")) {
					female = true;
				}
			}

			if(desc_changed) {
				QString desc = Qt::escape(description);
				const int maxDescLength = 40;

				if(female) {
					notifyText += tr("changed hers description");
				} else {
					notifyText += tr("changed his description");
				}

				if(desc.length() > 0) {
					notifyText += "<br>\"";
					if(desc.length() <= maxDescLength) {
						notifyText += desc;
					} else {
						notifyText += desc.left(maxDescLength) + "...";
					}
					notifyText += "\"";
				}
			} else if(status_changed) {
				switch(status) {
					case KittySDK::IProtocol::Online:
					{
						if(female) {
							notifyText += tr("is online", "female");
						} else {
							notifyText += tr("is online", "male");
						}
					}
					break;

					case KittySDK::IProtocol::Away:
					{
						if(female) {
							notifyText += tr("is away", "female");
						} else {
							notifyText += tr("is away", "male");
						}
					}
					break;

					case KittySDK::IProtocol::FFC:
					{
						if(female) {
							notifyText += tr("is free for chat", "female");
						} else {
							notifyText += tr("is free for chat", "male");
						}
					}
					break;

					case KittySDK::IProtocol::DND:
					{
						if(female) {
							notifyText += tr("shouldn't be disturbed", "female");
						} else {
							notifyText += tr("shouldn't be disturbed", "male");
						}
					}
					break;

					case KittySDK::IProtocol::Offline:
					{
						if(female) {
							notifyText += tr("is offline", "female");
						} else {
							notifyText += tr("is offline", "male");
						}
					}
					break;

					default:
						notifyText += tr("is wtf");
					break;
				}
			}

			notifyText += "</span>";

			if(KittySDK::IProtocol *proto = cnt->protocol()) {
				if(KittySDK::IPluginCore *core = proto->core()) {
					QMap<QString, QVariant> notifyArgs;
					notifyArgs.insert("icon", core->icon(proto->statusIcon(status)));
					notifyArgs.insert("text", notifyText);
					PluginManager::inst()->execAction("notify", "addNotify", notifyArgs);
				}
			}
		}
	}

	emit statusUpdated();
}


}
