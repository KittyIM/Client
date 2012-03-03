#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include "Singleton.h"

#include <IChat.h>

#include <QtCore/QObject>

bool contactCompare(KittySDK::IContact *left, KittySDK::IContact *right);

namespace Kitty
{
	class ChatManager: public QObject, public Singleton<ChatManager>
	{
		Q_OBJECT

		friend class Singleton<ChatManager>;

		public:
			const QList<KittySDK::IChat*> &chats() const;
			const QList<KittySDK::IChat*> chatsByAccount(KittySDK::IAccount *account) const;

			KittySDK::IChat *chat(KittySDK::IContact *me, const QList<KittySDK::IContact*> &contacts) const;
			KittySDK::IChat *chat(KittySDK::IContact *me, KittySDK::IContact *sender) const;
			KittySDK::IChat *chat(const QString &chatId) const;

			void startChat(KittySDK::IContact *me, const QList<KittySDK::IContact*> &contacts);
			KittySDK::IChat *createChat(KittySDK::IContact *me, const QList<KittySDK::IContact*> &contacts);

		public slots:
			void receiveMessage(KittySDK::IMessage &msg);
			void receiveTypingNotify(KittySDK::IContact *contact, bool typing, const int &length);

		private:
			ChatManager(QObject *parent = 0): QObject(parent) { }
			~ChatManager();

		private:
			QList<KittySDK::IChat*> m_chats;
	};
}
#endif // CHATMANAGER_H
