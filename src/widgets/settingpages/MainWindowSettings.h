#ifndef MAINWINDOWSETTING_H
#define MAINWINDOWSETTING_H

#include <ISettingsPage.h>

namespace Ui
{
	class MainWindowSettings;
}

namespace Kitty
{
	class Core;

	class MainWindowSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit MainWindowSettings(Core *core, QWidget *parent = 0);
			~MainWindowSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::MainWindowSettings *m_ui;
			Core *m_core;
	};
}

#endif // MAINWINDOWSETTING_H
