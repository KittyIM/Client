TARGET        = Kitty
DESTDIR       = ../bin
TEMPLATE      = app

QT           += core gui xml network webkit sql

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

include(3rdparty/3rdparty.pri)
include(widgets/widgets.pri)
include(SDK/KittySDK.pri)

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
