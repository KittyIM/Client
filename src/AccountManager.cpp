#include "AccountManager.h"

#include "widgets/windows/MainWindow.h"
#include "3rdparty/json/json.h"
#include "ProtocolManager.h"
#include "ContactManager.h"
#include "PluginManager.h"
#include "MessageQueue.h"
#include "ChatManager.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QAction>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[AccountManager]"
#define qWarning() qWarning() << "[AccountManager]"

namespace Kitty
{

AccountManager::AccountManager(Core *core):
	QObject(core),
	m_core(core)
{
}

AccountManager::~AccountManager()
{
	qDeleteAll(m_accounts);
}

const QList<KittySDK::IAccount*> &AccountManager::accounts() const
{
	return m_accounts;
}

const QList<KittySDK::IAccount*> AccountManager::accountsByProtocol(KittySDK::IProtocol *protocol) const
{
	QList<KittySDK::IAccount*> accounts;

	foreach(KittySDK::IAccount *account, m_accounts) {
		if(account->protocol() == protocol) {
			accounts.append(account);
		}
	}

	return accounts;
}

KittySDK::IAccount *AccountManager::account(KittySDK::IProtocol *protocol, const QString &uid) const
{
	foreach(KittySDK::IAccount *account, accountsByProtocol(protocol)) {
		if(account->uid() == uid) {
			return account;
		}
	}

	return 0;
}

KittySDK::IAccount *AccountManager::account(const QString &protocol, const QString &uid) const
{
	if(KittySDK::IProtocol *proto = ProtocolManager::inst()->protocolByName(protocol)) {
		foreach(KittySDK::IAccount *account, accountsByProtocol(proto)) {
			if(account->uid() == uid) {
				return account;
			}
		}
	}

	return 0;
}

bool AccountManager::add(KittySDK::IAccount *account)
{
	//qDebug() << "Adding new account" << account->uid() << account->password() << account->protocol()->protoInfo()->protoName();

	foreach(KittySDK::IAccount *acc, accountsByProtocol(account->protocol())) {
		if(account->uid() == acc->uid()) {
			return false;
		}
	}

	connect(account, SIGNAL(statusChanged(KittySDK::IProtocol::Status,QString)), this, SLOT(notifyStatusChange(KittySDK::IProtocol::Status,QString)));
	connect(account, SIGNAL(messageReceived(KittySDK::IMessage&)), m_core->chatManager(), SLOT(receiveMessage(KittySDK::IMessage&)));
	connect(account, SIGNAL(messageReceived(KittySDK::IMessage&)), m_core->messageQueue(), SLOT(enqueue(KittySDK::IMessage&)));
	connect(account, SIGNAL(contactAdded(KittySDK::IContact*)), m_core->contactManager(), SLOT(add(KittySDK::IContact*)));
	connect(account, SIGNAL(typingNotifyReceived(KittySDK::IContact*,bool,int)), m_core->chatManager(), SLOT(receiveTypingNotify(KittySDK::IContact*,bool,int)));

	if(account->protocol()->abilities() & KittySDK::IProtocol::ChangeStatus) {
		QAction *action = new QAction(this);
		action->setText(QString("%1 (%2)").arg(account->uid()).arg(account->protocol()->protoInfo()->protoName()));

		if(account->description().length() > 0) {
			action->setToolTip(QString("%1<br>%2").arg(action->text()).arg(account->description()));
		} else {
			action->setToolTip(action->text());
		}

		action->setIcon(Core::inst()->icon(account->protocol()->statusIcon(account->status())));
		action->setProperty("protocol", account->protocol()->protoInfo()->protoName());
		action->setProperty("uid", account->uid());
		//connect(account, SIGNAL(statusChanged(KittySDK::IProtocol::Status, QString)), Core::inst()->mainWindow(), SLOT(updateAccountStatusIcon()));
		connect(action, SIGNAL(triggered()), Core::inst()->mainWindow(), SLOT(showAccountStatusMenu()));

		Core::inst()->mainWindow()->addToolbarAction(KittySDK::Toolbars::TB_NETWORKS, action);
	}

	m_accounts.append(account);

	emit accountAdded();

	return true;
}

void AccountManager::load(const QString &profile)
{
	//qDebug() << "Loading accounts for" << profile;

	QFile file(Core::inst()->profilesDir() + profile + "/accounts.dat");
	if(file.exists()) {
		if(file.open(QIODevice::ReadOnly)) {
			QVariantMap map = Json::parse(qUncompress(file.readAll())).toMap();
			if(map.contains("accounts")) {
				QVariantList list = map.value("accounts").toList();
				foreach(const QVariant &item, list) {
					QVariantMap settings = item.toMap();

					if(settings.contains("protocol")) {
						if(KittySDK::IProtocol *proto = ProtocolManager::inst()->protocolByName(settings.value("protocol").toString())) {
							Plugin *plug = m_core->pluginManager()->pluginById(proto->info()->id());
							if(plug->isLoaded()) {
								if(KittySDK::IAccount *acc = proto->newAccount(settings.value("uid").toString())) {
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
				qWarning() << "No accounts info!";
			}

			file.close();
		} else {
			qWarning() << "Could not open file!";
		}
	}

	emit allLoaded();
}

void AccountManager::save(const QString &profile)
{
	//qDebug() << "saving accounts for" << profile;

	emit aboutToSave();

	QVariantList list;
	foreach(KittySDK::IAccount *account, m_accounts) {
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

void AccountManager::changeDescription(const QString &description)
{
	foreach(KittySDK::IAccount *acc, m_accounts) {
		if(acc->status() != KittySDK::IProtocol::Offline) {
			acc->changeStatus(acc->status(), description);
		}
	}
}

void AccountManager::changeStatus(KittySDK::IProtocol::Status status)
{
	foreach(KittySDK::IAccount *acc, m_accounts) {
		acc->changeStatus(status, acc->description());
	}
}

void AccountManager::notifyStatusChange(KittySDK::IProtocol::Status status, const QString &description)
{
	if(KittySDK::IAccount *account = dynamic_cast<KittySDK::IAccount*>(sender())) {
		emit accountStatusChanged(account, status, description);
	}
}

}
