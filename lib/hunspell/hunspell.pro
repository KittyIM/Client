QT       -= core gui

TARGET = hunspell
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    utf_info.cxx \
    suggestmgr.cxx \
    replist.cxx \
    phonet.cxx \
    hunzip.cxx \
    hunspell.cxx \
    hashmgr.cxx \
    filemgr.cxx \
    dictmgr.cxx \
    csutil.cxx \
    affixmgr.cxx \
    affentry.cxx

HEADERS += \
    hunvisapi.h \
    hunspell.h \
    config.h \
    w_char.hxx \
    suggestmgr.hxx \
    replist.hxx \
    phonet.hxx \
    langnum.hxx \
    hunzip.hxx \
    hunspell.hxx \
    htypes.hxx \
    hashmgr.hxx \
    filemgr.hxx \
    dictmgr.hxx \
    csutil.hxx \
    baseaffix.hxx \
    atypes.hxx \
    affixmgr.hxx \
    affentry.hxx
