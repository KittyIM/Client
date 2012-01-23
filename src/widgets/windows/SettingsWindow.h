#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtGui/QDialog>

class QTreeWidgetItem;

namespace Ui
{
	class SettingsWindow;
}

namespace KittySDK
{
	class SettingPage;
}

namespace Kitty
{
	class SettingsWindow: public QDialog
	{
		Q_OBJECT

		public:
			explicit SettingsWindow(QWidget *parent = 0);
			~SettingsWindow();

			QTreeWidgetItem *itemById(const QString &id);
			KittySDK::SettingPage *pageById(const QString &id);
			void addPage(KittySDK::SettingPage *page, const QString &parent = QString());

			void resetSettings();

			void addDefaultPages();

		public slots:
			void updateIcons();

		signals:
			void settingsApplied();

		protected:
			void showEvent(QShowEvent *event);
			void changeEvent(QEvent *event);

		private slots:
			void applySettings();
			void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
			void on_buttonBox_accepted();
			void on_buttonBox_rejected();

		private:
			Ui::SettingsWindow *m_ui;
			QList<KittySDK::SettingPage*> m_pages;
	};
}

#endif // SETTINGSWINDOW_H
