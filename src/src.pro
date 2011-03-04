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
    XmlSettings.cpp

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
    XmlSettings.h

FORMS      += widgets/DebugWindow.ui \
              widgets/MainWindow.ui \
              widgets/ProfilesWindow.ui

RESOURCES  += ../res.qrc
RC_FILE     = ../res.rc
