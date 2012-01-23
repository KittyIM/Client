#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include "SDK/SettingPage.h"

class QTreeWidgetItem;

namespace Ui
{
	class AccountsSettings;
}

namespace Kitty
{
	class AccountsSettings: public KittySDK::SettingPage
	{
		Q_OBJECT

		public:
			explicit AccountsSettings(QWidget *parent = 0);
			~AccountsSettings();

			void retranslate();
			void apply();
			void reset();

		private slots:
			void refreshAccounts();
			void addAccount();
			void on_addButton_clicked();
			void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
			void on_editButton_clicked();

		private:
			Ui::AccountsSettings *m_ui;
	};
}

#endif // ACCOUNTSSETTINGS_H
