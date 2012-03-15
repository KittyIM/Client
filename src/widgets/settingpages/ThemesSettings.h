#ifndef THEMESSETTINGS_H
#define THEMESSETTINGS_H

#include <ISettingsPage.h>

namespace Ui
{
	class ThemesSettings;
}

namespace Kitty
{
	class Core;

	class ThemesSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			ThemesSettings(Core *core, QWidget *parent = 0);
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
			Core *m_core;
	};
}

#endif // THEMESSETTINGS_H
