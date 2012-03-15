#include "ChatManager.h"

#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatTab.h"
#include "PluginManager.h"
#include "MessageQueue.h"
#include "Core.h"

#include <SoundsConstants.h>
#include <IContact.h>
#include <IMessage.h>

#include <QtCore/QCryptographicHash>
#include <QtGui/QTextDocument>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[ChatManager]"
#define qWarning() qWarning() << "[ChatManager]"

bool contactCompare(KittySDK::IContact *left, KittySDK::IContact *right)
{
	return left->uid().toLower() < right->uid().toLower();
}

namespace Kitty
{

ChatManager::ChatManager(Core *core):
	QObject(core),
	m_core(core)
{
}

ChatManager::~ChatManager()
{
	qDeleteAll(m_chats);
}

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

KittySDK::IChat *ChatManager::chat(const QString &chatId) const
{
	foreach(KittySDK::IChat *chat, m_chats) {
		if(chat->id() == chatId) {
			return chat;
		}
	}

	return 0;
}

void ChatManager::startChat(KittySDK::IContact *me, const QList<KittySDK::IContact*> &contacts)
{
	KittySDK::IChat *ch = chat(me, contacts);
	if(!ch) {
		ch = createChat(me, contacts);
	}

	m_core->chatWindow()->showChat(ch);
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
	QList<Plugin*> plugins = m_core->pluginManager()->plugins();
	foreach(Plugin *plugin, plugins) {
		if(plugin->isLoaded()) {
			plugin->iplugin()->receiveMessage(msg);
		}
	}

	if((msg.direction() == KittySDK::IMessage::Incoming) || (msg.direction() == KittySDK::IMessage::System)) {
		QList<KittySDK::IContact*> contacts = msg.to();
		KittySDK::IContact *me = contacts.takeFirst();
		contacts.prepend(msg.from());

		QMap<QString, QVariant> soundsArgs;

		bool notify = false;

		//get/create chat
		KittySDK::IChat *ch = chat(me, contacts);
		if(!ch) {
			ch = createChat(me, contacts);
			notify = true;
			soundsArgs.insert("id", Sounds::Sounds::S_MSG_RECV_FIRST);
		} else {
			soundsArgs.insert("id", Sounds::Sounds::S_MSG_RECV);
		}

		msg.setChat(ch);

		//see if we even need a notify
		ChatWindow *chatWindow = m_core->chatWindow();
		if(!notify) {
			if(!chatWindow->isVisible()) {
				notify = true;
			} else {
				if(!chatWindow->isChatActive(ch)) {
					notify = true;
				}
			}
		}

		//only 1 notify per chat
		if(notify) {
			if(m_core->messageQueue()->incomingForChat(ch) > 0) {
				notify = false;
			}
		}

		//show the notify
		if(notify) {
			const int maxBodyLength = 40;

			QString notifyText = "<a href=\"core://openChat?chatId=" + ch->id() + "\"><span class=\"notifyText\">";
			notifyText += tr("Message from") + " ";
			notifyText += "<b>" + Qt::escape(msg.from()->display()) + "</b></span>";
			notifyText += "<br><span class=\"notifyLink\">\"";

			QString plain = msg.body();
			plain.remove(QRegExp("<[^>]*>"));

			if(plain.length() > maxBodyLength) {
				notifyText += plain.left(maxBodyLength) + "...";
			} else {
				notifyText += plain;
			}

			notifyText += "\"</span></a>";

			QMap<QString, QVariant> notifyArgs;
			notifyArgs.insert("icon", m_core->icon(KittySDK::Icons::I_MESSAGE));
			notifyArgs.insert("text", notifyText);
			m_core->pluginManager()->execAction("notify", "addNotify", notifyArgs);
		}

		//let the music play!
		m_core->pluginManager()->execAction("sounds", "playSound", soundsArgs);

		//show the window
		ChatTab *tab = chatWindow->startChat(ch);
		tab->appendMessage(msg);
	}

	//maaaybe blink the taskbar
	if(msg.direction() == KittySDK::IMessage::Incoming) {
		QApplication::alert(m_core->chatWindow());
	}
}

void ChatManager::receiveTypingNotify(KittySDK::IContact *contact, bool typing, const int &length)
{
	KittySDK::IChat *ch = chat(contact->account()->me(), contact);
	if(ch) {
		ChatTab *tab = m_core->chatWindow()->tabByChat(ch);
		if(tab) {
			tab->setTypingNotify(typing, length);
		}
	}
}

}
