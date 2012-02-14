#include "ChatManager.h"

#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatTab.h"
#include "PluginManager.h"
#include "Core.h"

#include <IContact.h>
#include <IMessage.h>

#define qDebug() qDebug() << "[ChatManager]"
#define qWarning() qWarning() << "[ChatManager]"

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
	foreach(KittySDK::IChat *chat, chatsByAccount(me->account())) {
		if(chat->contacts() == contacts) {
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
		ch = new KittySDK::IChat(me, contacts, qrand());
		m_chats.append(ch);
	}

	Core::inst()->chatWindow()->startChat(ch);
	Core::inst()->chatWindow()->switchTo(ch);
	Core::inst()->chatWindow()->show();
	Core::inst()->chatWindow()->activateWindow();
}

void ChatManager::receiveMessage(KittySDK::IMessage &msg)
{
	QList<Plugin*> plugins = PluginManager::inst()->plugins();
	foreach(Plugin *plugin, plugins) {
		if(plugin->isLoaded()) {
			plugin->plugin()->receiveMessage(msg);
		}
	}

	if((msg.direction() == KittySDK::IMessage::Incoming) || (msg.direction() == KittySDK::IMessage::System)) {
		QList<KittySDK::IContact*> contacts = msg.to();
		KittySDK::IContact *me = contacts.takeFirst();
		contacts.prepend(msg.from());

		KittySDK::IChat *ch = chat(me, contacts);
		if(!ch) {
			ch = new KittySDK::IChat(me, contacts, qrand());
			m_chats.append(ch);
		}

		msg.setChat(ch);

		ChatTab *tab = Core::inst()->chatWindow()->startChat(ch);
		tab->appendMessage(msg);

		Core::inst()->chatWindow()->switchTo(ch);
		Core::inst()->chatWindow()->show();
		Core::inst()->chatWindow()->activateWindow();
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
