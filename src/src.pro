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
    widgets/PortabilityWindow.cpp

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
    SDK/SettingPage.h \
    widgets/PortabilityWindow.h

FORMS      += widgets/DebugWindow.ui \
              widgets/MainWindow.ui \
              widgets/ProfilesWindow.ui \
              widgets/AboutWindow.ui \
    widgets/SettingsWindow.ui \
    widgets/PortabilityWindow.ui

RESOURCES  += ../res.qrc
RC_FILE     = ../res.rc
