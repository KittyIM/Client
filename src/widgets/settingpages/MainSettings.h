#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class MainSettings;
}

namespace Kitty
{
  class MainSettings: public KittySDK::SettingPage
  {
    Q_OBJECT

    public:
      explicit MainSettings(QWidget *parent = 0);
      ~MainSettings();

      void apply();
      void reset();

    private:
      Ui::MainSettings *m_ui;
  };

}

#endif // MAINSETTINGS_H
