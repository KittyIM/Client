#ifndef CHATWINDOWTABSSETTINGS_H
#define CHATWINDOWTABSSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ChatWindowTabsSettings;
}

namespace Kitty
{
	class ChatWindowTabsSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit ChatWindowTabsSettings(QWidget *parent = 0);
			~ChatWindowTabsSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::ChatWindowTabsSettings *m_ui;
	};
}

#endif // CHATWINDOWTABSSETTINGS_H
