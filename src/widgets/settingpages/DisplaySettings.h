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

      void updateIcons();

    private slots:
      void on_mainWindowCaptionHelpButton_clicked();
      void on_chatWindowCaptionHelpButton_clicked();
      void on_chatTabCaptionHelpButton_clicked();

    private:
      Ui::DisplaySettings *m_ui;
  };
}

#endif // DISPLAYSETTINGS_H
