#ifndef ROSTERSETTINGS_H
#define ROSTERSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class RosterSettings;
}

namespace Kitty
{
	class RosterSettings: public KittySDK::ISettingsPage
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
