#include "ChatManager.h"

#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatTab.h"
#include "PluginManager.h"
#include "Core.h"

#include <IContact.h>
#include <IMessage.h>

#include <QtCore/QCryptographicHash>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[ChatManager]"
#define qWarning() qWarning() << "[ChatManager]"

bool contactCompare(KittySDK::IContact *left, KittySDK::IContact *right)
{
	return left->uid().toLower() < right->uid().toLower();
}

namespace Kitty
{

const QList<KittySDK::IChat*> &ChatManager::chats() const
{
	return m_chats;
}

const QList<KittySDK::IChat*> ChatManager::chatsByAccount(KittySDK::IAccount *account) const
{
	QList<KittySDK::IChat*> chats;

	foreach(KittySDK::IChat *chat, m_chats) {
		if(chat->account() == account) {
			chats.append(chat);
		}
	}

	return chats;
}

KittySDK::IChat *ChatManager::chat(KittySDK::IContact *me, const QList<KittySDK::IContact*> &contacts) const
{
	QList<KittySDK::IContact*> sortedContacts = contacts;
	qSort(sortedContacts.begin(), sortedContacts.end(), contactCompare);

	foreach(KittySDK::IChat *chat, chatsByAccount(me->account())) {
		QList<KittySDK::IContact*> chatContacts = chat->contacts();
		qSort(chatContacts.begin(), chatContacts.end(), contactCompare);

		if(chatContacts == sortedContacts) {
			return chat;
		}
	}

	return 0;
}

KittySDK::IChat *ChatManager::chat(KittySDK::IContact *me, KittySDK::IContact *sender) const
{
	return chat(me, QList<KittySDK::IContact*>() << sender);
}

void ChatManager::startChat(KittySDK::IContact *me, const QList<KittySDK::IContact*> &contacts)
{
	KittySDK::IChat *ch = chat(me, contacts);
	if(!ch) {
		ch = createChat(me, contacts);
	}

	Core::inst()->chatWindow()->showChat(ch);
}

KittySDK::IChat *ChatManager::createChat(KittySDK::IContact *me, const QList<KittySDK::IContact *> &contacts)
{
	QString seed = contacts.first()->uid() + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());

	KittySDK::IChat *ch = new KittySDK::IChat(me, contacts, QCryptographicHash::hash(seed.toAscii(), QCryptographicHash::Sha1).toHex());
	m_chats.append(ch);

	return ch;
}

void ChatManager::receiveMessage(KittySDK::IMessage &msg)
{
	QList<Plugin*> plugins = PluginManager::inst()->plugins();
	foreach(Plugin *plugin, plugins) {
		if(plugin->isLoaded()) {
			plugin->iplugin()->receiveMessage(msg);
		}
	}

	if((msg.direction() == KittySDK::IMessage::Incoming) || (msg.direction() == KittySDK::IMessage::System)) {
		QList<KittySDK::IContact*> contacts = msg.to();
		KittySDK::IContact *me = contacts.takeFirst();
		contacts.prepend(msg.from());

		KittySDK::IChat *ch = chat(me, contacts);
		if(!ch) {
			ch = createChat(me, contacts);
		}

		msg.setChat(ch);

		ChatTab *tab = Core::inst()->chatWindow()->startChat(ch);
		tab->appendMessage(msg);

	}

	if(msg.direction() == KittySDK::IMessage::Incoming) {
		QApplication::alert(Core::inst()->chatWindow());
	}
}

void ChatManager::receiveTypingNotify(KittySDK::IContact *contact, bool typing, const int &length)
{
	KittySDK::IChat *ch = chat(contact->account()->me(), contact);
	if(ch) {
		ChatTab *tab = Core::inst()->chatWindow()->tabByChat(ch);
		if(tab) {
			tab->setTypingNotify(typing, length);
		}
	}
}

ChatManager::~ChatManager()
{
	qDeleteAll(m_chats);
}

}
