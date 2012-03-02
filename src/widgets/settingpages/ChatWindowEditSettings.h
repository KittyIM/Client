#ifndef KITTY_CHATWINDOWEDITSETTINGS_H
#define KITTY_CHATWINDOWEDITSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ChatWindowEditSettings;
}

namespace Kitty
{
	class ChatWindowEditSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit ChatWindowEditSettings(QWidget *parent = 0);
			~ChatWindowEditSettings();

			void retranslate();
			void apply();
			void reset();

		public slots:
			void updateFontSizes();

		private:
			Ui::ChatWindowEditSettings *m_ui;
	};
}

#endif // KITTY_CHATWINDOWEDITSETTINGS_H
