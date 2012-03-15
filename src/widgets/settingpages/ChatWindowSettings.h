#ifndef CHATWINDOWSETTINGS_H
#define CHATWINDOWSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ChatWindowSettings;
}

namespace Kitty
{
	class Core;

	class ChatWindowSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit ChatWindowSettings(Core *core, QWidget *parent = 0);
			~ChatWindowSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::ChatWindowSettings *m_ui;
			Core *m_core;
	};
}

#endif // CHATWINDOWSETTINGS_H
