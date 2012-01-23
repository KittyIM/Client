#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
	class HistorySettings;
}

namespace Kitty
{
	class HistorySettings: public KittySDK::SettingPage
	{
		Q_OBJECT

		public:
			explicit HistorySettings(QWidget *parent = 0);
			~HistorySettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::HistorySettings *m_ui;
	};
}

#endif // HISTORYSETTINGS_H
