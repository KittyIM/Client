#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class DisplaySettings;
}

namespace Kitty
{
	class DisplaySettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit DisplaySettings(QWidget *parent = 0);
			~DisplaySettings();

			void retranslate();
			void apply();
			void reset();

			void updateIcons();

		private slots:
			void refreshAccounts();
			void on_mainWindowCaptionHelpButton_clicked();
			void on_chatWindowCaptionHelpButton_clicked();
			void on_chatTabCaptionHelpButton_clicked();

		private:
			Ui::DisplaySettings *m_ui;
	};
}

#endif // DISPLAYSETTINGS_H
