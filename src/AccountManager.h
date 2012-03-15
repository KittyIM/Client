#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <IAccount.h>

namespace Kitty
{
	class Core;

	class AccountManager: public QObject
	{
		Q_OBJECT

		public:
			AccountManager(Core *core);
			~AccountManager();

			const QList<KittySDK::IAccount*> &accounts() const;
			const QList<KittySDK::IAccount*> accountsByProtocol(KittySDK::IProtocol *protocol) const;

			KittySDK::IAccount *account(KittySDK::IProtocol *protocol, const QString &uid) const;
			KittySDK::IAccount *account(const QString &protocol, const QString &uid) const;

			bool add(KittySDK::IAccount *account);

			void load(const QString &profile);
			void save(const QString &profile);

		public slots:
			void changeDescription(const QString &description);
			void changeStatus(KittySDK::IProtocol::Status status);

		signals:
			void accountAdded();
			void allLoaded();
			void aboutToSave();
			void accountStatusChanged(KittySDK::IAccount *account, KittySDK::IProtocol::Status status, const QString &description);

		private slots:
			void notifyStatusChange(KittySDK::IProtocol::Status status, const QString &description);

		private:
			QList<KittySDK::IAccount*> m_accounts;
			Core *m_core;
	};
}

#endif // ACCOUNTMANAGER_H
