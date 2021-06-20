greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 no_batch
# 防止linux下程序名中空格造成的问题，分开处理
VERSION = 2.3.2

# 此处控制在windows下编译qml版还是widget版
# 被注释时编译widget版
# 未注释时编译qml版
#DEFINES += COMPILE_QML

win32 {
    QT += core
    RC_ICONS = "Key Container.ico"
    TARGET = "Key Container"
    LIBS +=  "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\User32.Lib"
    SOURCES += \
        cml/keymapjsonengine.cpp \
        ui/animationrefresh.cpp \
        commoninclude.cpp \
        debugshowoptions.cpp \
        encryption/encrypted.cpp \
        encryption/qaesencryption.cpp \
        kcdb.cpp \
        main.cpp \

    HEADERS += \
        cml/keymapjsonengine.h \
        ui/animationrefresh.h \
        commoninclude.h \
        debugshowoptions.h \
        encryption/encrypted.h \
        encryption/qaesencryption.h \
        kcdb.h \
}

if(contains(DEFINES, COMPILE_QML)){
    win32{
        QT += quick qml
        SOURCES += cml/qmlimporter.cpp
        HEADERS += cml/qmlimporter.h
        RESOURCES += qmlresource.qrc
        DEFINES += ENABLE_QML
    }
}
else{
    win32{
        QT += gui
        LIBS +=  "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\User32.Lib"
        SOURCES += \
            findkeyui.cpp \
            globalshortcut/globalshortcut.cpp \
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
            findkeyui.h \
            globalshortcut/globalshortcut.h \
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

        RESOURCES += \
            resource.qrc
    }
}

win32-msvc* {
  QMAKE_CXXFLAGS += /utf-8
  QMAKE_CXXFLAGS += /MP
}

unix:!android {
    QT += core gui quick qml
    RC_ICONS = "KeyContainer.ico"
    TARGET = "KeyContainer"
    SOURCES += \
        cml/keymapjsonengine.cpp \
        ui/animationrefresh.cpp \
        commoninclude.cpp \
        debugshowoptions.cpp \
        encryption/encrypted.cpp \
        encryption/qaesencryption.cpp \
        findkeyui.cpp \
        globalshortcut/globalshortcut.cpp \
        inputinitkeyui.cpp \
        inputkeyui.cpp \
        kcdb.cpp \
        login.cpp \
        main.cpp \
        mainui.cpp \
        qssinstaller.cpp \
        ui/messageboxexx.cpp \
        ui/qlineeditpro.cpp \
        ui/tablewidgetex.cpp \
        ui/titlebar.cpp \
        uistyle/proxystyle.cpp

    HEADERS += \
        cml/keymapjsonengine.h \
        ui/animationrefresh.h \
        commoninclude.h \
        debugshowoptions.h \
        encryption/encrypted.h \
        encryption/qaesencryption.h \
        findkeyui.h \
        globalshortcut/globalshortcut.h \
        inputinitkeyui.h \
        inputkeyui.h \
        kcdb.h \
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
    RESOURCES += \
        resource.qrc
}

android {
    QT += quick qml androidextras
    SOURCES += \
    cml/keymapjsonengine.cpp \
    cml/qmlimporter.cpp \
    commoninclude.cpp \
    debugshowoptions.cpp \
    encryption/encrypted.cpp \
    encryption/qaesencryption.cpp \
    main.cpp \
    kcdb.cpp

    HEADERS += \
    cml/keymapjsonengine.h \
    cml/qmlimporter.h \
    commoninclude.h \
    debugshowoptions.h \
    encryption/encrypted.h \
    encryption/qaesencryption.h \
    kcdb.h

    RESOURCES += \
        qmlresource.qrc
}
#LIBS +=  D:\WindowsKits\Lib\10.0.18362.0\um\x64\WS2_32.lib
#LIBS +=  "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\User32.Lib"

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

DISTFILES += \
    android/AndroidManifest.xml \
    android/src/com/th000/keycontainer/IOData.java \
    android/src/com/th000/keycontainer/SelfApplication.java \
    log

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

