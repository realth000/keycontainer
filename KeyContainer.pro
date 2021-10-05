greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 no_batch
# 防止linux下程序名中空格造成的问题，分开处理
VERSION = 3.1.4

# common config
QT += core
SOURCES += \
    cml/keymapjsonengine.cpp \
    commoninclude.cpp \
    debugshowoptions.cpp \
    encryption/encrypted.cpp \
    encryption/qaesencryption.cpp \
    kcdb.cpp \
    main.cpp

HEADERS += \
    cml/keymapjsonengine.h \
    commoninclude.h \
    debugshowoptions.h \
    encryption/encrypted.h \
    encryption/qaesencryption.h \
    kcdb.h

RESOURCES += \
    resource.qrc

DISTFILES += \
    log

# windows config
win32 {
    RC_ICONS = "Key Container.ico"
    TARGET = "Key Container"
    LIBS +=  "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\User32.Lib"
}

# linux config
unix:!android {
    RC_ICONS = "KeyContainer.ico"
    TARGET = "KeyContainer"
    LIBS += -lX11
    QMAKE_CXXFLAGS += -std=c++1z
}

if(contains(DEFINES, COMPILE_QML)) {
    # QML config
    QT += quick qml
    SOURCES += cml/qmlimporter.cpp
    HEADERS += cml/qmlimporter.h
    RESOURCES += qmlresource.qrc
    DEFINES += ENABLE_QML
}
else {
    # Wigets config
    QT += gui
    SOURCES += \
        ui/animationrefresh.cpp \
        findkeyui.cpp \
        inputinitkeyui.cpp \
        inputkeyui.cpp \
        login.cpp \
        mainui.cpp \
        qssinstaller.cpp \
        ui/messageboxexx.cpp \
        ui/qlineeditpro.cpp \
        ui/tablewidgetex.cpp \
        ui/titlebar.cpp \
        uistyle/proxystyle.cpp

    HEADERS += \
        ui/animationrefresh.h \
        findkeyui.h \
        inputinitkeyui.h \
        inputkeyui.h \
        login.h \
        mainui.h \
        qssinstaller.h \
        ui/messageboxexx.h \
        ui/qlineeditpro.h \
        ui/tablewidgetex.h \
        ui/titlebar.h \
        uistyle/proxystyle.h

    FORMS += \
        findkeyui.ui \
        inputinitkeyui.ui \
        inputkeyui.ui \
        login.ui \
        mainui.ui \
        ui/messageboxexx.ui
}

win32-msvc* {
  QMAKE_CXXFLAGS += /utf-8
# use -j N in project settings when using jom instead
#  QMAKE_CXXFLAGS += /MP
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS APP_VERSION=\\\"$$VERSION\\\"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES += \
#    android/AndroidManifest.xml \
#    android/src/com/th000/keycontainer/IOData.java \
#    android/src/com/th000/keycontainer/SelfApplication.java \
#    login.ec

#contains(ANDROID_TARGET_ARCH,arm64-v8a) {
#    ANDROID_PACKAGE_SOURCE_DIR = \
#        $$PWD/android
#}

