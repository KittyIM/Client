#ifndef ROSTERSETTINGS_H
#define ROSTERSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
	class RosterSettings;
}

namespace Kitty
{
	class RosterSettings: public KittySDK::SettingPage
	{
		Q_OBJECT

		public:
			explicit RosterSettings(QWidget *parent = 0);
			~RosterSettings();

			void retranslate();
			void apply();
			void reset();

		private slots:
			void on_descriptionComboBox_currentIndexChanged(int index);

		private:
			Ui::RosterSettings *m_ui;
	};
}

#endif // ROSTERSETTINGS_H
