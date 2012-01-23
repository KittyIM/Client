#ifndef PORTABILITYWINDOW_H
#define PORTABILITYWINDOW_H

#include <QtGui/QDialog>

namespace Ui
{
	class PortabilityWindow;
}

namespace Kitty
{
	class PortabilityWindow: public QDialog
	{
		Q_OBJECT

		public:
			explicit PortabilityWindow(QWidget *parent = 0);
			~PortabilityWindow();

		private:
			Ui::PortabilityWindow *m_ui;
	};
}

#endif // PORTABILITYWINDOW_H
