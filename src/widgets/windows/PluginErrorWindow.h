#ifndef PLUGINERRORWINDOW_H
#define PLUGINERRORWINDOW_H

#include <QtGui/QDialog>

class QListWidgetItem;

namespace Ui
{
	class PluginErrorWindow;
}

namespace Kitty
{
	class Core;

	class PluginErrorWindow: public QDialog
	{
		Q_OBJECT

		public:
			PluginErrorWindow(Core *core, QWidget *parent = 0);
			~PluginErrorWindow();

		private slots:
			void showInfo(QListWidgetItem *current, QListWidgetItem *);

		private:
			Ui::PluginErrorWindow *m_ui;
			Core *m_core;
	};
}

#endif // PLUGINERRORWINDOW_H
