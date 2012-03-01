#ifndef MAINWINDOWSETTING_H
#define MAINWINDOWSETTING_H

#include <ISettingsPage.h>

namespace Ui
{
	class MainWindowSettings;
}

namespace Kitty
{
	class MainWindowSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit MainWindowSettings(QWidget *parent = 0);
			~MainWindowSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::MainWindowSettings *m_ui;
	};
}

#endif // MAINWINDOWSETTING_H
