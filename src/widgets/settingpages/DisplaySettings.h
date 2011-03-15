#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class DisplaySettings;
}

namespace Kitty
{
  class DisplaySettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit DisplaySettings(QWidget *parent = 0);
      ~DisplaySettings();

      void apply();
      void reset();

    private:
      Ui::DisplaySettings *m_ui;
  };
}

#endif // DISPLAYSETTINGS_H
