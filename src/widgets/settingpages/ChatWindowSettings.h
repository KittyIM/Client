#ifndef CHATWINDOWSETTINGS_H
#define CHATWINDOWSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
	class ChatWindowSettings;
}

namespace Kitty
{
	class ChatWindowSettings: public KittySDK::SettingPage
	{
		Q_OBJECT

		public:
			explicit ChatWindowSettings(QWidget *parent = 0);
			~ChatWindowSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::ChatWindowSettings *m_ui;
	};
}

#endif // CHATWINDOWSETTINGS_H
