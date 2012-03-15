#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include <IProtocol.h>

#include <QtCore/QObject>

namespace Kitty
{
	class Core;

	class ProtocolManager: public QObject
	{
		Q_OBJECT

		public:
			ProtocolManager(Core *core);
			~ProtocolManager();

			const QList<KittySDK::IProtocol*> &protocols() const;
			KittySDK::IProtocol *protocolByName(const QString &name) const;

			void add(KittySDK::IProtocol *protocol);

		private:
			QList<KittySDK::IProtocol*> m_protocols;
			Core *m_core;
	};
}

#endif // PROTOCOLMANAGER_H
