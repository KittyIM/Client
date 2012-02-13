TARGET        = Kitty
DESTDIR       = ../bin
TEMPLATE      = app

QT           += core gui xml network webkit sql

SOURCES      += main.cpp \
                3rdparty/qtwin/qtwin.cpp \
                App.cpp \
                ActionManager.cpp \
                Core.cpp \
                IconManager.cpp \
                Profile.cpp \
                IconTheme.cpp \
                PluginManager.cpp \
                PluginCoreImpl.cpp \
                widgets/windows/ChatWindow.cpp \
                widgets/windows/AboutWindow.cpp \
                widgets/windows/SettingsWindow.cpp \
                widgets/windows/PortabilityWindow.cpp \
                widgets/windows/DebugWindow.cpp \
                widgets/windows/MainWindow.cpp \
                widgets/windows/ProfilesWindow.cpp \
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
                widgets/settingpages/PluginsSettings.cpp \
                widgets/ChatEdit.cpp \
                widgets/ChatTab.cpp \
                widgets/ChatTabWidget.cpp \
                RosterItemModel.cpp \
                RosterItemDelegate.cpp \
                RosterItem.cpp \
                widgets/RosterTreeView.cpp \
                ProtocolManager.cpp \
                ContactManager.cpp \
                AccountManager.cpp \
                RosterContact.cpp \
                RosterSortProxy.cpp \
                RosterGroup.cpp \
                JsonSettings.cpp \
                3rdparty/json/json.cpp \
                ChatManager.cpp \
                ChatTheme.cpp \
                widgets/ChatWebView.cpp \
                widgets/ChatColorPicker.cpp \
                widgets/RosterHeader.cpp \
                widgets/StatusChangeButton.cpp \
                widgets/StatusTextWidget.cpp \
                widgets/windows/ContactWindow.cpp \
                EmoticonManager.cpp \
                EmoticonPack.cpp \
                widgets/windows/HistoryWindow.cpp \
                widgets/DesktopFragmentDialog.cpp

HEADERS      += constants.h \
                SDK/constants.h \
                SDK/SettingPage.h \
                SDK/Protocol.h \
                SDK/Plugin.h \
                SDK/PluginCore.h \
                3rdparty/qtwin/qtwin.h \
                App.h \
                ActionManager.h \
                Core.h \
                IconManager.h \
                Profile.h \
                IconTheme.h \
                widgets/windows/ChatWindow.h \
                widgets/windows/DebugWindow.h \
                widgets/windows/MainWindow.h \
                widgets/windows/ProfilesWindow.h \
                widgets/windows/AboutWindow.h \
                widgets/windows/SettingsWindow.h \
                widgets/windows/PortabilityWindow.h \
                Singleton.h \
                PluginManager.h \
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
                widgets/settingpages/PluginsSettings.h \
                widgets/ChatEdit.h \
                widgets/ChatTab.h \
                widgets/ChatTabWidget.h \
                RosterItemModel.h \
                RosterItemDelegate.h \
                RosterItem.h \
                widgets/RosterTreeView.h \
                AbstractTheme.h \
                ProtocolManager.h \
                ContactManager.h \
                SDK/Account.h \
                AccountManager.h \
                SDK/Contact.h \
                RosterContact.h \
                RosterSortProxy.h \
                RosterGroup.h \
                JsonSettings.h \
                3rdparty/json/json.h \
                SDK/Message.h \
                SDK/Chat.h \
                ChatManager.h \
                ChatTheme.h \
                widgets/ChatWebView.h \
                widgets/ChatColorPicker.h \
                widgets/RosterHeader.h \
                widgets/StatusChangeButton.h \
                widgets/StatusTextWidget.h \
                widgets/windows/ContactWindow.h \
                EmoticonManager.h \
                EmoticonPack.h \
                widgets/windows/HistoryWindow.h \
                widgets/DesktopFragmentDialog.h

FORMS        += widgets/windows/DebugWindow.ui \
                widgets/windows/MainWindow.ui \
                widgets/windows/ProfilesWindow.ui \
                widgets/windows/AboutWindow.ui \
                widgets/windows/SettingsWindow.ui \
                widgets/windows/PortabilityWindow.ui \
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
                widgets/settingpages/PluginsSettings.ui \
                widgets/windows/ChatWindow.ui \
                widgets/ChatTab.ui \
                widgets/RosterHeader.ui \
                widgets/windows/ContactWindow.ui \
                widgets/windows/HistoryWindow.ui

RESOURCES    += ../res/res.qrc
RC_FILE       = ../res.rc

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
