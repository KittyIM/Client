SOURCES      += $$PWD/ChatColorPicker.cpp \
                $$PWD/ChatEdit.cpp \
                $$PWD/ChatTab.cpp \
                $$PWD/ChatTabWidget.cpp \
                $$PWD/ChatWebView.cpp \
                $$PWD/DesktopFragmentDialog.cpp \
                $$PWD/RosterHeader.cpp \
                $$PWD/RosterTreeView.cpp \
                $$PWD/StatusChangeButton.cpp \
                $$PWD/StatusTextWidget.cpp

HEADERS      += $$PWD/ChatColorPicker.h \
                $$PWD/ChatEdit.h \
                $$PWD/ChatTab.h \
                $$PWD/ChatTabWidget.h \
                $$PWD/ChatWebView.h \
                $$PWD/DesktopFragmentDialog.h \
                $$PWD/RosterHeader.h \
                $$PWD/RosterTreeView.h \
                $$PWD/StatusChangeButton.h \
                $$PWD/StatusTextWidget.h

FORMS        += $$PWD/ChatTab.ui \
                $$PWD/RosterHeader.ui

include(windows/windows.pri)
include(settingpages/settingpages.pri)
