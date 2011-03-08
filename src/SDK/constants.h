#ifndef SDKCONSTANTS_H
#define SDKCONSTANTS_H

namespace KittySDK
{
  namespace Icons
  {
    const char * const I_KITTY          = "Kitty.Icon";
    const char * const I_QUIT           = "Kitty.Quit";
    const char * const I_SETTINGS       = "Kitty.Settings";
    const char * const I_PLUGIN         = "Kitty.Plugin";
    const char * const I_INFO           = "Kitty.Info";
    const char * const I_PROFILE        = "Kitty.Profile";
    const char * const I_USER           = "Kitty.User";
    const char * const I_FOLDER         = "Kitty.Folder";
    const char * const I_FOLDER_KITTY   = "Kitty.KittyFolder";
    const char * const I_REFRESH        = "Kitty.Refresh";
    const char * const I_CONSOLE        = "Kitty.Console";
    const char * const I_BULLET         = "Kitty.Bullet";
    const char * const I_CONNECT        = "Kitty.Connect";
    const char * const I_HISTORY        = "Kitty.History";
    const char * const I_KEY            = "Kitty.Key";
    const char * const I_PALETTE        = "Kitty.Palette";
    const char * const I_MESSAGE        = "Kitty.Message";
    const char * const I_SMILIE         = "Kitty.Smilie";
    const char * const I_TAB            = "Kitty.Tab";
  }

  namespace Actions
  {
    const char * const A_QUIT           = "Kitty.Quit";
    const char * const A_SHOW_HIDE      = "Kitty.ShowHide";
    const char * const A_SETTINGS       = "Kitty.Settings";
    const char * const A_RESTART        = "Kitty.Restart";
    const char * const A_KITTY_FOLDER   = "Kitty.KittyFolder";
    const char * const A_PROFILE_FOLDER = "Kitty.ProfileFolder";
    const char * const A_ABOUT          = "Kitty.About";
    const char * const A_DEBUG          = "Kitty.Debug";
    const char * const A_PLUGINS        = "Kitty.Plugins";
  }

  namespace Settings
  {
    const char * const S_MAINWINDOW_TRANSPARENCY       = "Kitty.MainWindow.Transparency.Enabled";
    const char * const S_MAINWINDOW_TRANSPARENCY_VALUE = "Kitty.MainWindow.Transparency.Value";
    const char * const S_MAINWINDOW_GEOMETRY           = "Kitty.Geometries.MainWindow";
    const char * const S_MAINWINDOW_STATE              = "Kitty.MainWindow.State";
    const char * const S_MAINWINDOW_STARTHIDDEN        = "Kitty.MainWindow.StartHidden";
    const char * const S_MAINWINDOW_CAPTION            = "Kitty.Display.Captions.MainWindow";

    const char * const S_DEBUGWINDOW_GEOMETRY          = "Kitty.Geometries.DebugWindow";
    const char * const S_SETTINGSWINDOW_GEOMETRY       = "Kitty.Geometries.SettingsWindow";

    const char * const S_PROFILE_PASSWORD              = "Profile.Password";
    const char * const S_PROFILE_THEMES_ICON           = "Profile.Themes.Icon";

  }

  namespace SettingPages
  {
    const char * const S_SETTINGS                   = "Settings";
    const char * const S_SETTINGS_STARTUP           = "Settings.Startup";
    const char * const S_SETTINGS_CONNECTION        = "Settings.Connection";
    const char * const S_USER                       = "User";
    const char * const S_USER_ACCOUNTS              = "User.Accounts";
    const char * const S_DISPLAY                    = "Display";
    const char * const S_DISPLAY_THEMES             = "Display.Themes";
    const char * const S_DISPLAY_ROSTER             = "Display.Roster";
    const char * const S_DISPLAY_HISTORY            = "Display.History";
    const char * const S_DISPLAY_CHATWINDOW         = "Display.ChatWindow";
    const char * const S_DISPLAY_CHATWINDOW_TABS    = "Display.ChatWindow.Tabs";
    const char * const S_DISPLAY_CHATWINDOW_SMILIES = "Display.ChatWindow.Smilies";
    const char * const S_PLUGINS                    = "Plugins";



  }
}

#endif // SDKCONSTANTS_H
