#ifndef THEMESSETTINGS_H
#define THEMESSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class ThemesSettings;
}

namespace Kitty
{
  class ThemesSettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit ThemesSettings(QWidget *parent = 0);
      ~ThemesSettings();

      void apply();
      void reset();

    private:
      Ui::ThemesSettings *m_ui;
  };
}

#endif // THEMESSETTINGS_H