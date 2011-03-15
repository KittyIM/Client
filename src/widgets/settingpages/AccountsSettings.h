#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include "SDK/SettingPage.h"

namespace Ui
{
  class AccountsSettings;
}

namespace Kitty
{
  class AccountsSettings: public KittySDK::SettingPage
  {
      Q_OBJECT

    public:
      explicit AccountsSettings(QWidget *parent = 0);
      ~AccountsSettings();

      void apply();
      void reset();

    private:
      Ui::AccountsSettings *m_ui;
  };
}

#endif // ACCOUNTSSETTINGS_H
