#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ConnectionSettings;
}

namespace Kitty
{
	class Core;

	class ConnectionSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			ConnectionSettings(Core *core, QWidget *parent = 0);
			~ConnectionSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::ConnectionSettings *m_ui;
			Core *m_core;
	};
}

#endif // CONNECTIONSETTINGS_H
