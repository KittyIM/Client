#include "MessageQueue.h"

#include "widgets/windows/ChatWindow.h"
#include "3rdparty/json/json.h"
#include "AccountManager.h"
#include "ContactManager.h"
#include "ChatManager.h"
#include "Core.h"

#include <IChat.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>

#define qDebug() qDebug() << "[MessageQueue]"
#define qWarning() qWarning() << "[MessageQueue]"

namespace KittySDK
{
	bool operator ==(const IMessage& left, const IMessage& right)
	{
		return (left.from() == right.from()) && (left.to() == right.to()) &&
				(left.chat() == right.chat()) && (left.body() == right.body()) &&
				(left.timeStamp() == right.timeStamp()) && (left.direction() == right.direction());
	}
}

namespace Kitty
{

MessageQueue::MessageQueue(Core *core):
	QObject(core),
	m_core(core),
	m_nextId(0)
{
	connect(m_core->accountManager(), SIGNAL(accountStatusChanged(KittySDK::IAccount*,KittySDK::IProtocol::Status,QString)), SLOT(checkAccountQueue(KittySDK::IAccount*,KittySDK::IProtocol::Status)));
}

void MessageQueue::load(const QString &profile)
{
	//qDebug() << "loading queue" << profile;

	QFile file(Core::inst()->profilesDir() + profile + "/queues.json");
	if(file.exists()) {
		if(file.open(QIODevice::ReadOnly)) {
			qDebug() << "1";
			QVariantMap map = Json::parse(/*qUncompress(*/file.readAll())/*)*/.toMap();

			if(map.contains("incoming")) {
				qDebug() << "2";
				QVariantList incoming = map.value("incoming").toList();
				foreach(const QVariant &var, incoming) {
					qDebug() << "3";
					QVariantMap item = var.toMap();

					if(KittySDK::IAccount *acc = Core::inst()->accountManager()->account(item.value("protocol").toString(), item.value("account").toString())) {
						qDebug() << "4";
						if(KittySDK::IContact *from = Core::inst()->contactManager()->contact(acc, item.value("from").toString())) {
							qDebug() << "5";
							QList<KittySDK::IContact*> to;

							if(!item.value("to").toList().isEmpty()) {
								QString uid = item.value("to").toList().first().toString();

								if(uid == item.value("account").toString()) {
									to << acc->me();
								}
							}

							foreach(const QVariant &uid, item.value("to").toList()) {
								if(KittySDK::IContact *cnt = Core::inst()->contactManager()->contact(acc, uid.toString())) {
									to << cnt;
								}
							}

							if(to.count()) {
								qDebug() << "6";

								KittySDK::IMessage *msg = new KittySDK::IMessage(from, to);
								msg->setBody(item.value("body").toString());
								msg->setTimeStamp(item.value("timeStamp").toDateTime());
								msg->setDirection(static_cast<KittySDK::IMessage::Direction>(item.value("direction").toInt()));

								quint32 msgId = item.value("msgId").toUInt();

								m_queue.insert(msgId, msg);
								Core::inst()->chatManager()->receiveMessage(*msg);
								emit messageEnqueued(msgId, *msg);

								m_nextId = qMax(m_nextId, msgId);
							}
						}
					}

				}
			}
		}
	}
}

void MessageQueue::save(const QString &profile)
{
	qDebug() << "saving queue";

	QVariantMap queueMap;

	QVariantList outgoing;
	QVariantList incoming;

	QMapIterator<quint32, KittySDK::IMessage*> it(m_queue);
	while(it.hasNext()) {
		it.next();

		QVariantMap map = MessageQueue::messageToMap(it.value());
		map.insert("msgId", it.key());

		switch(it.value()->direction()) {
			case KittySDK::IMessage::Outgoing:
				outgoing << map;
			break;

			case KittySDK::IMessage::Incoming:
				incoming << map;
			break;

			default:
			break;
		}
	}

	queueMap.insert("outgoing", outgoing);
	queueMap.insert("incoming", incoming);

	QFile file(Core::inst()->profilesDir() + profile + "/queues.json");
	if(file.open(QIODevice::ReadWrite)) {
		file.resize(0);
		file.write(/*qCompress(*/Json::stringify(queueMap).toAscii()/*)*/);
		file.close();
	}
}

quint32 MessageQueue::idByMsg(const KittySDK::IMessage &msg)
{
	QMapIterator<quint32, KittySDK::IMessage*> it(m_queue);
	while(it.hasNext()) {
		it.next();

		if(*(it.value()) == msg) {
			return it.key();
		}
	}

	return 0;
}

int MessageQueue::incomingCount() const
{
	int count = 0;

	QMapIterator<quint32, KittySDK::IMessage*> it(m_queue);
	while(it.hasNext()) {
		it.next();

		if(it.value()->direction() == KittySDK::IMessage::Incoming) {
			++count;
		}
	}

	return count;
}

bool MessageQueue::isFirstForChat(KittySDK::IChat *chat)
{
	return (incomingForChat(chat) == 1);
}

int MessageQueue::incomingForChat(KittySDK::IChat *chat)
{
	int count = 0;

	QMapIterator<quint32, KittySDK::IMessage*> it(m_queue);
	while(it.hasNext()) {
		it.next();

		if(it.value()->direction() == KittySDK::IMessage::Incoming) {
			if(it.value()->chat() == chat) {
				++count;
			}
		}
	}

	return count;
}

QVariantMap MessageQueue::messageToMap(KittySDK::IMessage *msg)
{
	QVariantMap map;

	map.insert("body", msg->body());
	map.insert("timeStamp", msg->timeStamp());
	map.insert("direction", msg->direction());
	map.insert("protocol", msg->chat()->protocol()->protoInfo()->protoName());
	map.insert("account", msg->chat()->account()->uid());
	map.insert("from", msg->from()->uid());

	QVariantList to;
	foreach(KittySDK::IContact *cnt, msg->to()) {
		to << cnt->uid();
	}

	map.insert("to", to);

	return map;
}

quint32 MessageQueue::enqueue(KittySDK::IMessage &msg)
{
	//qDebug() << "enqueue";

	KittySDK::IMessage *mess = new KittySDK::IMessage(msg.from(), msg.to(), msg.body(), msg.timeStamp(), msg.direction());
	mess->setChat(msg.chat());

	++m_nextId;
	m_queue.insert(m_nextId, mess);

	emit messageEnqueued(m_nextId, msg);

	return m_nextId;
}

quint32 MessageQueue::enqueue(const KittySDK::IMessage &msg)
{
	return enqueue(const_cast<KittySDK::IMessage&>(msg));
}

void MessageQueue::dequeue(KittySDK::IChat *chat)
{
	if(chat) {
		dequeue(chat->id());
	}
}

void MessageQueue::dequeue(const quint32 &msgId)
{
	if(KittySDK::IMessage *msg = m_queue.value(msgId)) {
		dequeue(msg->chat());
	}
}

void MessageQueue::dequeue(const QString &chatId)
{
	//qDebug() << "dequeue";

	QMutableMapIterator<quint32, KittySDK::IMessage*> it(m_queue);
	while(it.hasNext()) {
		it.next();

		if(it.value()->direction() == KittySDK::IMessage::Incoming) {
			if(it.value()->chat()->id() == chatId) {
				quint32 msgId = it.key();

				delete it.value();
				it.remove();

				messageDequeued(msgId);
			}
		}
	}
}

void MessageQueue::dequeueAndShow(const quint32 &msgId)
{
	if(KittySDK::IMessage *msg = m_queue.value(msgId)) {
		Core::inst()->chatWindow()->showChat(msg->chat());
	}
}

void MessageQueue::checkAccountQueue(KittySDK::IAccount *account, KittySDK::IProtocol::Status status)
{
	QMutableMapIterator<quint32, KittySDK::IMessage*> it(m_queue);
	while(it.hasNext()) {
		it.next();

		if(it.value()->direction() == KittySDK::IMessage::Outgoing) {
			if(it.value()->chat()->account() == account) {
				account->sendMessage(*(it.value()));

				delete it.value();
				it.remove();
			}
		}
	}
}

}
