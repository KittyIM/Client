#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ConnectionSettings;
}

namespace Kitty
{
	class ConnectionSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit ConnectionSettings(QWidget *parent = 0);
			~ConnectionSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::ConnectionSettings *m_ui;
	};
}

#endif // CONNECTIONSETTINGS_H
