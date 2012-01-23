#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QtGui/QDialog>

namespace Ui
{
	class AboutWindow;
}

namespace Kitty
{
	class AboutWindow: public QDialog
	{
		Q_OBJECT

		public:
			explicit AboutWindow(QWidget *parent = 0);
			~AboutWindow();

		protected:
			void showEvent(QShowEvent *event);

		private:
			Ui::AboutWindow *m_ui;
	};
}

#endif // ABOUTWINDOW_H
