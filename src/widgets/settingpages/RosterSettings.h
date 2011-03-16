#ifndef ROSTERSETTINGS_H
#define ROSTERSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class RosterSettings;
}

namespace Kitty
{
  class RosterSettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit RosterSettings(QWidget *parent = 0);
      ~RosterSettings();

      void apply();
      void reset();

    private:
      Ui::RosterSettings *m_ui;
  };
}

#endif // ROSTERSETTINGS_H