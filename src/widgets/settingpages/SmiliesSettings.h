#ifndef SMILIESSETTINGS_H
#define SMILIESSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class SmiliesSettings;
}

namespace Kitty
{
  class SmiliesSettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit SmiliesSettings(QWidget *parent = 0);
      ~SmiliesSettings();

      void apply();
      void reset();

    private:
      Ui::SmiliesSettings *m_ui;
  };
}

#endif // SMILIESSETTINGS_H
