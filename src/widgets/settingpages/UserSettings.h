#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class UserSettings;
}

namespace Kitty
{
  class UserSettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit UserSettings(QWidget *parent = 0);
      ~UserSettings();

      void retranslate();
      void apply();
      void reset();

    private:
      Ui::UserSettings *m_ui;
  };
}

#endif // USERSETTINGS_H
