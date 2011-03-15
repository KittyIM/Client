TARGET      = Kitty
DESTDIR     = ../bin
TEMPLATE    = app

QT         += core gui xml network webkit

SOURCES    += main.cpp \
              qtwin/qtwin.cpp \
              App.cpp \
              ActionManager.cpp \
              Core.cpp \
              IconManager.cpp \
              Profile.cpp \
              widgets/DebugWindow.cpp \
              widgets/MainWindow.cpp \
              widgets/ProfilesWindow.cpp \
              XmlSettings.cpp \
              widgets/AboutWindow.cpp \
              IconTheme.cpp \
              widgets/SettingsWindow.cpp \
              widgets/PortabilityWindow.cpp \
              PluginManager.cpp \
    PluginCoreImpl.cpp \
    widgets/settingpages/MainSettings.cpp \
    widgets/settingpages/ConnectionSettings.cpp \
    widgets/settingpages/StartupSettings.cpp \
    widgets/settingpages/UserSettings.cpp \
    widgets/settingpages/AccountsSettings.cpp \
    widgets/settingpages/DisplaySettings.cpp \
    widgets/settingpages/ThemesSettings.cpp \
    widgets/settingpages/RosterSettings.cpp \
    widgets/settingpages/HistorySettings.cpp \
    widgets/settingpages/ChatWindowSettings.cpp \
    widgets/settingpages/ChatWindowTabsSettings.cpp \
    widgets/settingpages/SmiliesSettings.cpp \
    widgets/settingpages/PluginsSettings.cpp

HEADERS    += constants.h \
              SDK/constants.h \
              qtwin/qtwin.h \
              App.h \
              ActionManager.h \
              Core.h \
              IconManager.h \
              Profile.h \
              widgets/DebugWindow.h \
              widgets/MainWindow.h \
              widgets/ProfilesWindow.h \
              XmlSettings.h \
              widgets/AboutWindow.h \
              IconTheme.h \
              widgets/SettingsWindow.h \
              widgets/PortabilityWindow.h \
              Singleton.h \
              PluginManager.h \
              SDK/SettingPage.h \
              SDK/Protocol.h \
              SDK/Plugin.h \
              SDK/PluginCore.h \
    PluginCoreImpl.h \
    widgets/settingpages/MainSettings.h \
    widgets/settingpages/ConnectionSettings.h \
    widgets/settingpages/StartupSettings.h \
    widgets/settingpages/UserSettings.h \
    widgets/settingpages/AccountsSettings.h \
    widgets/settingpages/DisplaySettings.h \
    widgets/settingpages/ThemesSettings.h \
    widgets/settingpages/RosterSettings.h \
    widgets/settingpages/HistorySettings.h \
    widgets/settingpages/ChatWindowSettings.h \
    widgets/settingpages/ChatWindowTabsSettings.h \
    widgets/settingpages/SmiliesSettings.h \
    widgets/settingpages/PluginsSettings.h

FORMS      += widgets/DebugWindow.ui \
              widgets/MainWindow.ui \
              widgets/ProfilesWindow.ui \
              widgets/AboutWindow.ui \
              widgets/SettingsWindow.ui \
              widgets/PortabilityWindow.ui \
    widgets/settingpages/MainSettings.ui \
    widgets/settingpages/ConnectionSettings.ui \
    widgets/settingpages/StartupSettings.ui \
    widgets/settingpages/UserSettings.ui \
    widgets/settingpages/AccountsSettings.ui \
    widgets/settingpages/DisplaySettings.ui \
    widgets/settingpages/ThemesSettings.ui \
    widgets/settingpages/RosterSettings.ui \
    widgets/settingpages/HistorySettings.ui \
    widgets/settingpages/ChatWindowSettings.ui \
    widgets/settingpages/ChatWindowTabsSettings.ui \
    widgets/settingpages/SmiliesSettings.ui \
    widgets/settingpages/PluginsSettings.ui

RESOURCES  += ../res.qrc
RC_FILE     = ../res.rc
