TARGET        = Kitty
DESTDIR       = ../bin
TEMPLATE      = app

QT           += core gui xml network webkit sql

#main files
SOURCES      += main.cpp \
                AccountManager.cpp \
                ActionManager.cpp \
                App.cpp \
                ChatManager.cpp \
                ChatTheme.cpp \
                ContactManager.cpp \
                Core.cpp \
                EmoticonManager.cpp \
                EmoticonPack.cpp \
                IconManager.cpp \
                IconTheme.cpp \
                JsonSettings.cpp \
                PluginCoreImpl.cpp \
                PluginManager.cpp \
                Profile.cpp \
                ProtocolManager.cpp \
                RosterContact.cpp \
                RosterGroup.cpp \
                RosterItem.cpp \
                RosterItemDelegate.cpp \
                RosterItemModel.cpp \
                RosterSortProxy.cpp

HEADERS      += constants.h \
                AbstractTheme.h \
                AccountManager.h \
                ActionManager.h \
                App.h \
                ChatManager.h \
                ChatTheme.h \
                ContactManager.h \
                Core.h \
                EmoticonManager.h \
                EmoticonPack.h \
                IconManager.h \
                IconTheme.h \
                JsonSettings.h \
                PluginCoreImpl.h \
                PluginManager.h \
                Profile.h \
                ProtocolManager.h \
                RosterContact.h \
                RosterGroup.h \
                RosterItem.h \
                RosterItemDelegate.h \
                RosterItemModel.h \
                RosterSortProxy.h \
                Singleton.h

#3rd party
SOURCES      += 3rdparty/json/json.cpp \
                3rdparty/qtwin/qtwin.cpp

HEADERS      += 3rdparty/json/json.h \
                3rdparty/qtwin/qtwin.h

#widgets
SOURCES      += widgets/ChatColorPicker.cpp \
                widgets/ChatEdit.cpp \
                widgets/ChatTab.cpp \
                widgets/ChatTabWidget.cpp \
                widgets/ChatWebView.cpp \
                widgets/DesktopFragmentDialog.cpp \
                widgets/RosterHeader.cpp \
                widgets/RosterTreeView.cpp \
                widgets/StatusChangeButton.cpp \
                widgets/StatusTextWidget.cpp

HEADERS      += widgets/ChatColorPicker.h \
                widgets/ChatEdit.h \
                widgets/ChatTab.h \
                widgets/ChatTabWidget.h \
                widgets/ChatWebView.h \
                widgets/DesktopFragmentDialog.h \
                widgets/RosterHeader.h \
                widgets/RosterTreeView.h \
                widgets/StatusChangeButton.h \
                widgets/StatusTextWidget.h

FORMS        += widgets/ChatTab.ui \
                widgets/RosterHeader.ui

#windows
SOURCES      += widgets/windows/AboutWindow.cpp \
                widgets/windows/ChatWindow.cpp \
                widgets/windows/ContactWindow.cpp \
                widgets/windows/DebugWindow.cpp \
                widgets/windows/HistoryWindow.cpp \
                widgets/windows/MainWindow.cpp \
                widgets/windows/PortabilityWindow.cpp \
                widgets/windows/ProfilesWindow.cpp \
                widgets/windows/SettingsWindow.cpp

HEADERS      += widgets/windows/AboutWindow.h \
                widgets/windows/ChatWindow.h \
                widgets/windows/ContactWindow.h \
                widgets/windows/DebugWindow.h \
                widgets/windows/HistoryWindow.h \
                widgets/windows/MainWindow.h \
                widgets/windows/PortabilityWindow.h \
                widgets/windows/ProfilesWindow.h \
                widgets/windows/SettingsWindow.h

FORMS        += widgets/windows/AboutWindow.ui \
                widgets/windows/ChatWindow.ui \
                widgets/windows/ContactWindow.ui \
                widgets/windows/DebugWindow.ui \
                widgets/windows/HistoryWindow.ui \
                widgets/windows/MainWindow.ui \
                widgets/windows/PortabilityWindow.ui \
                widgets/windows/ProfilesWindow.ui \
                widgets/windows/SettingsWindow.ui

#settings pages
SOURCES      += widgets/settingpages/AccountsSettings.cpp \
                widgets/settingpages/ChatWindowSettings.cpp \
                widgets/settingpages/ChatWindowTabsSettings.cpp \
                widgets/settingpages/ConnectionSettings.cpp \
                widgets/settingpages/DisplaySettings.cpp \
                widgets/settingpages/HistorySettings.cpp \
                widgets/settingpages/MainSettings.cpp \
                widgets/settingpages/PluginsSettings.cpp \
                widgets/settingpages/RosterSettings.cpp \
                widgets/settingpages/SmiliesSettings.cpp \
                widgets/settingpages/StartupSettings.cpp \
                widgets/settingpages/ThemesSettings.cpp \
                widgets/settingpages/UserSettings.cpp

HEADERS      += widgets/settingpages/AccountsSettings.h \
                widgets/settingpages/ChatWindowSettings.h \
                widgets/settingpages/ChatWindowTabsSettings.h \
                widgets/settingpages/ConnectionSettings.h \
                widgets/settingpages/DisplaySettings.h \
                widgets/settingpages/HistorySettings.h \
                widgets/settingpages/MainSettings.h \
                widgets/settingpages/PluginsSettings.h \
                widgets/settingpages/RosterSettings.h \
                widgets/settingpages/SmiliesSettings.h \
                widgets/settingpages/StartupSettings.h \
                widgets/settingpages/ThemesSettings.h \
                widgets/settingpages/UserSettings.h

FORMS        += widgets/settingpages/AccountsSettings.ui \
                widgets/settingpages/ChatWindowSettings.ui \
                widgets/settingpages/ChatWindowTabsSettings.ui \
                widgets/settingpages/ConnectionSettings.ui \
                widgets/settingpages/DisplaySettings.ui \
                widgets/settingpages/HistorySettings.ui \
                widgets/settingpages/MainSettings.ui \
                widgets/settingpages/PluginsSettings.ui \
                widgets/settingpages/RosterSettings.ui \
                widgets/settingpages/SmiliesSettings.ui \
                widgets/settingpages/StartupSettings.ui \
                widgets/settingpages/ThemesSettings.ui \
                widgets/settingpages/UserSettings.ui

RESOURCES    += ../res/res.qrc
RC_FILE       = ../res.rc

include(SDK/KittySDK.pri)

LIBS         += -lhunspell

debug {
    LIBS     += -L../lib/hunspell/debug
} else {
    LIBS     += -L../lib/hunspell/release
}

windows {
    LIBS     += -lgdi32
}

QMAKE_LFLAGS += -static-libgcc
