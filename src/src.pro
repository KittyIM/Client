TARGET      = Kitty
DESTDIR     = ../bin
TEMPLATE    = app

QT         += core gui xml network webkit

SOURCES    += main.cpp \
              KittyApp.cpp \
              wndMain.cpp \
              KittyCore.cpp \
              wndDebug.cpp \
              KittyActionMngr.cpp \
              KittyIconMngr.cpp

HEADERS    += KittyApp.h \
              wndMain.h \
              KittyCore.h \
              wndDebug.h \
              constants.h \
              SDK/constants.h \
              KittyActionMngr.h \
              KittyIconMngr.h

FORMS      += wndMain.ui \
              wndDebug.ui

RESOURCES  += ../res.qrc
RC_FILE     = ../res.rc
LANGUAGES   = untranslated pl

include(translations.pri)
