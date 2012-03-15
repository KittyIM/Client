#ifndef PLUGINSSETTINGS_H
#define PLUGINSSETTINGS_H

#include <ISettingsPage.h>

class QTreeWidgetItem;

namespace Ui
{
	class PluginsSettings;
}

namespace Kitty
{
	class Core;

	class PluginsSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			explicit PluginsSettings(Kitty::Core *core, QWidget *parent = 0);
			~PluginsSettings();

			void retranslate();
			void apply();
			void reset();

		private slots:
			void on_pluginWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

		private:
			Ui::PluginsSettings *m_ui;
			Core *m_core;
	};
}

#endif // PLUGINSSETTINGS_H
