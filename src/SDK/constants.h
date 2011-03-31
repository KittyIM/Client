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
    const char * const I_SMILEY         = "Kitty.Smiley";
    const char * const I_TAB            = "Kitty.Tab";
    const char * const I_BOLD           = "Kitty.Bold";
    const char * const I_ITALIC         = "Kitty.Italic";
    const char * const I_UNDERLINE      = "Kitty.Underline";
    const char * const I_STRIKETROUGH   = "Kitty.Striketrough";
    const char * const I_COLOR          = "Kitty.Color";
    const char * const I_IMAGE          = "Kitty.Image";
    const char * const I_FILE           = "Kitty.File";

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
    const char * const S_LANGUAGE                      = "Kitty.Language";

    const char * const S_MAINWINDOW_TRANSPARENCY       = "Kitty.MainWindow.Transparency.Enabled";
    const char * const S_MAINWINDOW_TRANSPARENCY_VALUE = "Kitty.MainWindow.Transparency.Value";
    const char * const S_MAINWINDOW_GEOMETRY           = "Kitty.Geometries.MainWindow";
    const char * const S_MAINWINDOW_STATE              = "Kitty.MainWindow.State";
    const char * const S_MAINWINDOW_STARTHIDDEN        = "Kitty.MainWindow.StartHidden";
    const char * const S_MAINWINDOW_CAPTION            = "Kitty.Display.Captions.MainWindow";
    const char * const S_MAINWINDOW_ALWAYS_ON_TOP      = "Kitty.MainWindow.AlwaysOnTop";
    const char * const S_MAINWINDOW_AUTOHIDE           = "Kitty.MainWindow.AutoHide.Enabled";
    const char * const S_MAINWINDOW_AUTOHIDE_DELAY     = "Kitty.MainWindow.AutoHide.Delay";
    const char * const S_MAINWINDOW_DOCKING            = "Kitty.MainWindow.Docking.Enabled";
    const char * const S_MAINWINDOW_DOCKING_DISTANCE   = "Kitty.MainWindow.Docking.Distance";

    const char * const S_DEBUGWINDOW_GEOMETRY          = "Kitty.Geometries.DebugWindow";

    const char * const S_SETTINGSWINDOW_GEOMETRY       = "Kitty.Geometries.SettingsWindow";

    const char * const S_CHATWINDOW_CAPTION            = "Kitty.Display.Captions.ChatWindow";
    const char * const S_CHATWINDOW_GEOMETRY           = "Kitty.Geometries.ChatWindow";
    const char * const S_CHATWINDOW_SPELLCHECK_ENABLED = "Kitty.ChatWindow.SpellCheck.Enabled";
    const char * const S_CHATWINDOW_SPELLCHECK_DICT    = "Kitty.ChatWindow.SpellCheck.Dictionary";


    const char * const S_CHATTAB_CAPTION               = "Kitty.Display.Captions.ChatTab";

    const char * const S_PROFILE_PASSWORD              = "Profile.Password";

    const char * const S_ICON_THEME                    = "Profile.Themes.Icon";

    const char * const S_PROXY_ENABLED                 = "Kitty.Proxy.Enabled";
    const char * const S_PROXY_SERVER                  = "Kitty.Proxy.Server";
    const char * const S_PROXY_PORT                    = "Kitty.Proxy.Port";
    const char * const S_PROXY_AUTH                    = "Kitty.Proxy.Auth.Enabled";
    const char * const S_PROXY_USERNAME                = "Kitty.Proxy.Auth.Username";
    const char * const S_PROXY_PASSWORD                = "Kitty.Proxy.Auth.Password";

    const char * const S_ROSTER_THEME                  = "Profile.Themes.Roster";
    const char * const S_ROSTER_STATUS_DESCRIPTION     = "Kitty.Roster.StatusDescription";
    const char * const S_ROSTER_AVATARS                = "Kitty.Roster.Avatars";
    const char * const S_ROSTER_HIDE_OFFLINE           = "Kitty.Roster.HideOffline";
    const char * const S_ROSTER_GROUPS_AS_TABS         = "Kitty.Roster.GroupsAsTabs";
    const char * const S_ROSTER_GROUPS_ON_MAIN         = "Kitty.Roster.GroupsOnMain";
    const char * const S_ROSTER_PHONE_ICON             = "Kitty.Roster.Icons.Phone";
    const char * const S_ROSTER_EMAIL_ICON             = "Kitty.Roster.Icons.Email";
    const char * const S_ROSTER_TIPS                   = "Kitty.Roster.Tips.Enabled";
    const char * const S_ROSTER_TIPS_DELAY             = "Kitty.Roster.Tips.Delay";
    const char * const S_ROSTER_TIPS_IP                = "Kitty.Roster.Tips.IP";

    //const char * const S_                = "Kitty.";
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

  namespace Toolbars
  {
    const char * const TB_MAIN = "Toolbars.Main";
    const char * const TB_NETWORKS = "Toolbars.Networks";
    const char * const TB_PLUGINS = "Toolbars.Plugins";
  }
}

#endif // SDKCONSTANTS_H
