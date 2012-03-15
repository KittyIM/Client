#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class MainSettings;
}

namespace Kitty
{
	class Core;

	class MainSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			MainSettings(Core *core, QWidget *parent = 0);
			~MainSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::MainSettings *m_ui;
			Core *m_core;
	};

}

#endif // MAINSETTINGS_H
