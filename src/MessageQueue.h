#ifndef KITTY_MESSAGEQUEUE_H
#define KITTY_MESSAGEQUEUE_H

#include "Singleton.h"

#include <IProtocol.h>
#include <IMessage.h>

#include <QtCore/QVariant>

namespace Kitty
{

	class MessageQueue: public QObject, public Singleton<MessageQueue>
	{
		Q_OBJECT

		friend class Singleton<MessageQueue>;

		public:
			void load(const QString &profile);
			void save(const QString &profile);

			quint32 idByMsg(const KittySDK::IMessage &msg);

			int incomingCount() const;

			bool isFirstForChat(KittySDK::IChat *chat);
			int incomingForChat(KittySDK::IChat *chat);

			static QVariantMap messageToMap(KittySDK::IMessage *msg);

		signals:
			void messageEnqueued(quint32 msgId, const KittySDK::IMessage &msg);
			void messageDequeued(quint32 msgId);

		public slots:
			quint32 enqueue(KittySDK::IMessage &msg);
			quint32 enqueue(const KittySDK::IMessage &msg);
			void dequeue(KittySDK::IChat *chat);
			void dequeue(const quint32 &msgId);
			void dequeue(const QString &chatId);
			void dequeueAndShow(const quint32 &msgId);

		private slots:
			void checkAccountQueue(KittySDK::IAccount *account, KittySDK::IProtocol::Status status);

		private:
			MessageQueue(QObject *parent = 0);

		private:
			QMap<quint32, KittySDK::IMessage*> m_queue;
			quint32 m_nextId;
	};
}

#endif // KITTY_MESSAGEQUEUE_H
