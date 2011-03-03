#ifndef SDKCONSTANTS_H
#define SDKCONSTANTS_H

namespace KittySDK
{
  namespace Icons
  {
    enum
    {
      KITTY = 1,
      QUIT,
      SETTINGS,
      PLUGIN,
      INFO,
      PROFILE,
      USER,
      FOLDER,
      FOLDER_KITTY,
      REFRESH,
      CONSOLE,

      LAST
    };
  }

  namespace Actions
  {
    const char * const QUIT = "Kitty.Quit";
    const char * const SHOW_HIDE = "Kitty.ShowHide";
    const char * const SETTINGS = "Kitty.Settings";
    const char * const RESTART = "Kitty.Restart";
    const char * const OPEN_KITTY_FOLDER = "Kitty.OpenKittyFolder";
    const char * const OPEN_PROFILE_FOLDER = "Kitty.OpenProfileFolder";
    const char * const ABOUT = "Kitty.About";
    const char * const DEBUG = "Kitty.Debug";
    const char * const PLUGINS = "Kitty.Plugins";
  }
}

#endif // SDKCONSTANTS_H
