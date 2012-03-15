QT       += testlib

TARGET    = tst_SDK
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE  = app

SOURCES  += tst_SDK.cpp

include(../src/SDK/KittySDK.pri)
