#include "ProtocolManager.h"

#include <QtCore/QDebug>

const QList<KittySDK::Protocol*> &Kitty::ProtocolManager::protocols() const
{
  return m_protocols;
}

KittySDK::Protocol *Kitty::ProtocolManager::protocolByName(const QString &name) const
{
  foreach(KittySDK::Protocol *proto, m_protocols) {
    if(proto->protoInfo()->protoName() == name) {
      return proto;
    }
  }

  return 0;
}

void Kitty::ProtocolManager::add(KittySDK::Protocol *protocol)
{
  if(protocolByName(protocol->protoInfo()->protoName())) {
    qWarning() << "Tried to add protocol" << protocol->protoInfo()->protoName() << "which is already in ProtocolManager.";
    return;
  }

  m_protocols.append(protocol);
}



