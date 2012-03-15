#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class HistorySettings;
}

namespace Kitty
{
	class Core;

	class HistorySettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit HistorySettings(Core *core, QWidget *parent = 0);
			~HistorySettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::HistorySettings *m_ui;
			Core *m_core;
	};
}

#endif // HISTORYSETTINGS_H
