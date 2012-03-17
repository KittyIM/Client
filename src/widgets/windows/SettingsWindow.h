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
	class ISettingsPage;
}

namespace Kitty
{
	class Core;

	class SettingsWindow: public QDialog
	{
		Q_OBJECT

		public:
			SettingsWindow(Core *core, QWidget *parent = 0);
			~SettingsWindow();

			QTreeWidgetItem *itemById(const QString &id);
			KittySDK::ISettingsPage *pageById(const QString &id);

			void resetSettings();

			void addDefaultPages();
			void addPluginPages();

		public slots:
			void updateIcons();
			void addPage(KittySDK::ISettingsPage *page, const QString &parent = QString());

		signals:
			void settingsApplied();
			void languageChanged();

		protected:
			void showEvent(QShowEvent *event);
			void changeEvent(QEvent *event);

		private slots:
			void applySettings();
			void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
			void on_buttonBox_accepted();

		private:
			Ui::SettingsWindow *m_ui;
			Core *m_core;
			QList<KittySDK::ISettingsPage*> m_pages;
	};
}

#endif // SETTINGSWINDOW_H
