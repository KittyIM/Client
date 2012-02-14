#include "ProtocolManager.h"

#include <QtCore/QDebug>

#define qDebug() qDebug() << "[ProtocolManager]"
#define qWarning() qWarning() << "[ProtocolManager]"

namespace Kitty
{

ProtocolManager::~ProtocolManager()
{
	qDeleteAll(m_protocols);
}

const QList<KittySDK::IProtocol*> &ProtocolManager::protocols() const
{
	return m_protocols;
}

KittySDK::IProtocol *ProtocolManager::protocolByName(const QString &name) const
{
	foreach(KittySDK::IProtocol *proto, m_protocols) {
		if(proto->protoInfo()->protoName() == name) {
			return proto;
		}
	}

	return 0;
}

void ProtocolManager::add(KittySDK::IProtocol *protocol)
{
	if(protocolByName(protocol->protoInfo()->protoName())) {
		qWarning() << "Tried to add protocol" << protocol->protoInfo()->protoName() << "which is already in ProtocolManager.";
		return;
	}

	m_protocols.append(protocol);
}

}
