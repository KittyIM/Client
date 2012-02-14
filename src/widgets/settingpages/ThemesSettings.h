#ifndef THEMESSETTINGS_H
#define THEMESSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ThemesSettings;
}

namespace Kitty
{
	class ThemesSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit ThemesSettings(QWidget *parent = 0);
			~ThemesSettings();

			void retranslate();
			void apply();
			void reset();

		private slots:
			void updateChatPreview();
			void updateVariantList();
			void updateIconsPreview();

		private:
			Ui::ThemesSettings *m_ui;
	};
}

#endif // THEMESSETTINGS_H
