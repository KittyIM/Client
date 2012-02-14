#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class MainSettings;
}

namespace Kitty
{
	class MainSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			MainSettings(QWidget *parent = 0);
			~MainSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::MainSettings *m_ui;
	};

}

#endif // MAINSETTINGS_H
