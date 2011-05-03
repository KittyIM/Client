#include "ProtocolManager.h"

#include <QtCore/QDebug>

#define qDebug() qDebug() << "[ProtocolManager]"
#define qWarning() qWarning() << "[ProtocolManager]"

using namespace KittySDK;

const QList<Protocol*> &Kitty::ProtocolManager::protocols() const
{
  return m_protocols;
}

Protocol *Kitty::ProtocolManager::protocolByName(const QString &name) const
{
  foreach(Protocol *proto, m_protocols) {
    if(proto->protoInfo()->protoName() == name) {
      return proto;
    }
  }

  return 0;
}

void Kitty::ProtocolManager::add(Protocol *protocol)
{
  if(protocolByName(protocol->protoInfo()->protoName())) {
    qWarning() << "Tried to add protocol" << protocol->protoInfo()->protoName() << "which is already in ProtocolManager.";
    return;
  }

  m_protocols.append(protocol);
}



