#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class UserSettings;
}

namespace Kitty
{
	class Core;

	class UserSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			UserSettings(Core *core, QWidget *parent = 0);
			~UserSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::UserSettings *m_ui;
			Core *m_core;
	};
}

#endif // USERSETTINGS_H
