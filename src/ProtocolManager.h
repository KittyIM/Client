#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include "SDK/Protocol.h"
#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
  class ProtocolManager: public QObject, public Singleton<ProtocolManager>
  {
    Q_OBJECT

    friend class Singleton<ProtocolManager>;

    public:
      const QList<KittySDK::Protocol*> &protocols() const;
      KittySDK::Protocol *protocolByName(const QString &name) const;

      void add(KittySDK::Protocol *protocol);

    private:
      ProtocolManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<KittySDK::Protocol*> m_protocols;
  };
}

#endif // PROTOCOLMANAGER_H
