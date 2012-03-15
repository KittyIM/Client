#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <IChat.h>

bool contactCompare(KittySDK::IContact *left, KittySDK::IContact *right);

namespace Kitty
{
	class Core;

	class ChatManager: public QObject
	{
		Q_OBJECT

		public:
			ChatManager(Core *core);
			~ChatManager();

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
			Core *m_core;
			QList<KittySDK::IChat*> m_chats;
	};
}
#endif // CHATMANAGER_H
