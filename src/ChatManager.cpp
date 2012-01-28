#include "ChatManager.h"

#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatTab.h"
#include "PluginManager.h"
#include "SDK/Message.h"
#include "SDK/Contact.h"
#include "Core.h"

#define qDebug() qDebug() << "[ChatManager]"
#define qWarning() qWarning() << "[ChatManager]"

using namespace Kitty;
using namespace KittySDK;

const QList<Chat*> &Kitty::ChatManager::chats() const
{
	return m_chats;
}

const QList<Chat*> Kitty::ChatManager::chatsByAccount(Account *account) const
{
	QList<Chat*> chats;

	foreach(Chat *chat, m_chats) {
		if(chat->account() == account) {
			chats.append(chat);
		}
	}

	return chats;
}

Chat *Kitty::ChatManager::chat(Contact *me, const QList<Contact*> &contacts) const
{
	foreach(Chat *chat, chatsByAccount(me->account())) {
		if(chat->contacts() == contacts) {
			return chat;
		}
	}

	return 0;
}

KittySDK::Chat *Kitty::ChatManager::chat(KittySDK::Contact *me, KittySDK::Contact *sender) const
{
	return chat(me, QList<Contact*>() << sender);
}

void Kitty::ChatManager::startChat(Contact *me, const QList<Contact*> &contacts)
{
	Chat *ch = chat(me, contacts);
	if(!ch) {
		ch = new Chat(me, contacts, qrand());
		m_chats.append(ch);
	}

	Core::inst()->chatWindow()->startChat(ch);
	Core::inst()->chatWindow()->switchTo(ch);
	Core::inst()->chatWindow()->show();
	Core::inst()->chatWindow()->activateWindow();
}

void Kitty::ChatManager::receiveMessage(KittySDK::Message &msg)
{
	QList<Plugin*> plugins = PluginManager::inst()->plugins();
	foreach(Plugin *plugin, plugins) {
		if(plugin->isLoaded()) {
			plugin->plugin()->receiveMessage(msg);
		}
	}

	if((msg.direction() == Message::Incoming) || (msg.direction() == Message::System)) {
		QList<Contact*> contacts = msg.to();
		Contact *me = contacts.takeFirst();
		contacts.prepend(msg.from());

		Chat *ch = chat(me, contacts);
		if(!ch) {
			ch = new Chat(me, contacts, qrand());
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

void ChatManager::receiveTypingNotify(Contact *contact, bool typing, const int &length)
{
	Chat *ch = chat(contact->account()->me(), contact);
	if(ch) {
		ChatTab *tab = Core::inst()->chatWindow()->tabByChat(ch);
		if(tab) {
			tab->setTypingNotify(typing, length);
		}
	}
}

Kitty::ChatManager::~ChatManager()
{
	qDeleteAll(m_chats);
}
