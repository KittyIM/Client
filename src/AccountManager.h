#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "SDK/Account.h"
#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
	class AccountManager: public QObject, public Singleton<AccountManager>
	{
		Q_OBJECT

		friend class Singleton<AccountManager>;

		public:
			const QList<KittySDK::Account*> &accounts() const;
			const QList<KittySDK::Account*> accountsByProtocol(KittySDK::Protocol *protocol) const;

			KittySDK::Account *account(KittySDK::Protocol *protocol, const QString &uid) const;
			KittySDK::Account *account(const QString &protocol, const QString &uid) const;

			bool add(KittySDK::Account *account);

			void load(const QString &profile);
			void save(const QString &profile);

		public slots:
			void changeDescription(const QString &description);
			void changeStatus(KittySDK::Protocol::Status status);

		signals:
			void accountAdded();
			void accountStatusChanged(KittySDK::Account *account, KittySDK::Protocol::Status status, const QString &description);

		private slots:
			void notifyStatusChange(KittySDK::Protocol::Status status, const QString &description);

		private:
			AccountManager(QObject *parent = 0): QObject(parent) { }
			~AccountManager();

		private:
			QList<KittySDK::Account*> m_accounts;
	};
}

#endif // ACCOUNTMANAGER_H
