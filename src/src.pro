TARGET    = Kitty
DESTDIR   = ../bin
TEMPLATE  = app

QT       += core gui xml network

SOURCES  += main.cpp \ 
    KittyApp.cpp \
    wndMain.cpp \
    KittyCore.cpp

HEADERS  += \ 
    KittyApp.h \
    wndMain.h \
    KittyCore.h

FORMS    += \ 
    wndMain.ui

RESOURCES +=  ../res.qrc
RC_FILE    = ../res.rc
