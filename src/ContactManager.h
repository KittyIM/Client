#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include <IContact.h>

#include "Singleton.h"

namespace Kitty
{
	class ContactManager: public QObject, public Singleton<ContactManager>
	{
		Q_OBJECT

		friend class Singleton<ContactManager>;

		public:
			KittySDK::IContact *contact(KittySDK::IAccount *acc, const QString &uid) const;
			const QList<KittySDK::IContact*> &contacts() const;
			const QList<KittySDK::IContact*> contactsByProtocol(const QString &proto);
			const QList<KittySDK::IContact*> contactsByProtocol(KittySDK::IProtocol *proto);
			const QList<KittySDK::IContact*> contactsByAccount(const QString &acc, const QString &proto);
			const QList<KittySDK::IContact*> contactsByAccount(KittySDK::IAccount *acc) const;

			const QStringList groups() const;

			void load(const QString &profile);
			void save(const QString &profile);

		public slots:
			void add(KittySDK::IContact *contact);

		signals:
			void contactAdded(KittySDK::IContact *contact);
			void statusUpdated();

		private slots:
			void updateStatus(KittySDK::IProtocol::Status status, const QString &description);

		private:
			ContactManager(QObject *parent = 0): QObject(parent) { }
			~ContactManager();

		private:
			QList<KittySDK::IContact*> m_contacts;
	};
}

#endif // CONTACTMANAGER_H
