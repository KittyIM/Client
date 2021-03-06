#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include <ISettingsPage.h>

class QTreeWidgetItem;

namespace Ui
{
	class AccountsSettings;
}

namespace KittySDK
{
	class IAccount;
}

namespace Kitty
{
	class Core;

	class AccountsSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			AccountsSettings(Core *core, QWidget *parent = 0);
			~AccountsSettings();

			void retranslate();
			void apply();
			void reset();

		private slots:
			void refreshAccounts();
			void refreshAccount(KittySDK::IAccount *account);
			void addAccount();
			void on_addButton_clicked();
			void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
			void on_editButton_clicked();

		private:
			Ui::AccountsSettings *m_ui;
			Core *m_core;
	};
}

#endif // ACCOUNTSSETTINGS_H
