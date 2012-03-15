#ifndef CHATWINDOWTABSSETTINGS_H
#define CHATWINDOWTABSSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ChatWindowTabsSettings;
}

namespace Kitty
{
	class Core;

	class ChatWindowTabsSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			ChatWindowTabsSettings(Core *core, QWidget *parent = 0);
			~ChatWindowTabsSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::ChatWindowTabsSettings *m_ui;
			Core *m_core;
	};
}

#endif // CHATWINDOWTABSSETTINGS_H
