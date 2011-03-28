#include "ProtocolManager.h"

#include <QtCore/QDebug>

const QList<KittySDK::Protocol*> &Kitty::ProtocolManager::protocols() const
{
  return m_protocols;
}

void Kitty::ProtocolManager::add(KittySDK::Protocol *protocol)
{
  m_protocols.append(protocol);
}

