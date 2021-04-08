QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 no_batch

win32 {
    RC_ICONS = "Key Container.ico"
    TARGET = "Key Container"
    LIBS +=  D:\WindowsKits\Lib\10.0.18362.0\um\x64\User32.Lib
}
unix {
    RC_ICONS = "KeyContainer.ico"
    TARGET = "KeyContainer"
}

VERSION = 2.1.9

#LIBS +=  D:\WindowsKits\Lib\10.0.18362.0\um\x64\WS2_32.lib

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commoninclude.cpp \
    debugshowoptions.cpp \
    encryption/encrypted.cpp \
    encryption/qaesencryption.cpp \
    globalshortcut/globalshortcut.cpp \
    inputinitkeyui.cpp \
    inputkeyui.cpp \
    kcdb.cpp \
    login.cpp \
    main.cpp \
    mainui.cpp \
    qssinstaller.cpp \
    ui/qlineeditpro.cpp \
    ui/titlebar.cpp \
    uistyle/proxystyle.cpp

HEADERS += \
    commoninclude.h \
    debugshowoptions.h \
    encryption/encrypted.h \
    encryption/qaesencryption.h \
    globalshortcut/globalshortcut.h \
    inputinitkeyui.h \
    inputkeyui.h \
    kcdb.h \
    login.h \
    mainui.h \
    qssinstaller.h \
    ui/qlineeditpro.h \
    ui/titlebar.h \
    uistyle/proxystyle.h

FORMS += \
    inputinitkeyui.ui \
    inputkeyui.ui \
    login.ui \
    mainui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    log
