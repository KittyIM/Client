#ifndef STARTUPSETTINGS_H
#define STARTUPSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class StartupSettings;
}

namespace Kitty
{
	class Core;

	class StartupSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			StartupSettings(Core *core, QWidget *parent = 0);
			~StartupSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::StartupSettings *m_ui;
			Core *m_core;
	};
}

#endif // STARTUPSETTINGS_H
