#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include <IProtocol.h>
#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
	class ProtocolManager: public QObject, public Singleton<ProtocolManager>
	{
		Q_OBJECT

		friend class Singleton<ProtocolManager>;

		public:
			const QList<KittySDK::IProtocol*> &protocols() const;
			KittySDK::IProtocol *protocolByName(const QString &name) const;

			void add(KittySDK::IProtocol *protocol);

		private:
			ProtocolManager(QObject *parent = 0): QObject(parent) { }
			~ProtocolManager();

		private:
			QList<KittySDK::IProtocol*> m_protocols;
	};
}

#endif // PROTOCOLMANAGER_H
