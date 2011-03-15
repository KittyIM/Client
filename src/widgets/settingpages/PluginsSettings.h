#ifndef PLUGINSSETTINGS_H
#define PLUGINSSETTINGS_H

#include "SDK/SettingPage.h"

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

      void apply();
      void reset();

    private:
      Ui::PluginsSettings *m_ui;
  };
}

#endif // PLUGINSSETTINGS_H
