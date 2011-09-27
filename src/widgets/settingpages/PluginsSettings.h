#ifndef PLUGINSSETTINGS_H
#define PLUGINSSETTINGS_H

#include "SDK/SettingPage.h"

class QTreeWidgetItem;

namespace Ui
{
  class PluginsSettings;
}

namespace Kitty
{
  class PluginsSettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit PluginsSettings(QWidget *parent = 0);
      ~PluginsSettings();

      void retranslate();
      void apply();
      void reset();

    private slots:
      void on_pluginWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    private:
      Ui::PluginsSettings *m_ui;
  };
}

#endif // PLUGINSSETTINGS_H
