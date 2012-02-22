#ifndef PROFILESWINDOW_H
#define PROFILESWINDOW_H

#include <QtGui/QDialog>

class QTreeWidgetItem;

namespace Ui
{
	class ProfilesWindow;
}

namespace Kitty
{
	class ProfilesWindow: public QDialog
	{
		Q_OBJECT

		public:
			explicit ProfilesWindow(QWidget *parent = 0);
			~ProfilesWindow();

		protected:
			void showEvent(QShowEvent *event);
			void paintEvent(QPaintEvent *event);
			void keyPressEvent(QKeyEvent *event);
			void closeEvent(QCloseEvent *event);
			void changeEvent(QEvent *event);

		private slots:
			void on_profilesWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
			void on_profilesWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
			void on_loginButton_clicked();
			void on_deleteButton_clicked();

		private:
			Ui::ProfilesWindow *m_ui;
	};
}

#endif // PROFILESWINDOW_H
