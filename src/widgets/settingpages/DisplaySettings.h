#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class DisplaySettings;
}

namespace Kitty
{
	class Core;

	class DisplaySettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			DisplaySettings(Core *core, QWidget *parent = 0);
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
			Core *m_core;
	};
}

#endif // DISPLAYSETTINGS_H
