#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QtGui/QDialog>

namespace Ui
{
	class AboutWindow;
}

namespace Kitty
{
	class Core;

	class AboutWindow: public QDialog
	{
		Q_OBJECT

		public:
			AboutWindow(Core *core, QWidget *parent = 0);
			~AboutWindow();

		protected:
			void showEvent(QShowEvent *event);
			void changeEvent(QEvent *event);

		private:
			void updateAboutText();

		private:
			Ui::AboutWindow *m_ui;
			Core *m_core;
	};
}

#endif // ABOUTWINDOW_H
