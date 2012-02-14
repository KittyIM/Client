#ifndef STARTUPSETTINGS_H
#define STARTUPSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class StartupSettings;
}

namespace Kitty
{
	class StartupSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit StartupSettings(QWidget *parent = 0);
			~StartupSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::StartupSettings *m_ui;
	};
}

#endif // STARTUPSETTINGS_H
