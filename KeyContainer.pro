VERSION = 3.1.10
RC_ICONS = "src/resource/pic/KeyContainer.ico"

CONFIG += c++17 no_batch

INCLUDEPATH += src src/core src/utils

##########################
# COMPILE_QML
# COMPILE_VID

#DEFINES += COMPILE_VID
#DEFINES += COMPILE_QML
##########################

# common config
QT += core

SOURCES += \
    src/core/keymapjsonengine.cpp \
    src/core/commoninclude.cpp \
    src/core/debugshowoptions.cpp \
    src/core/encryption/encrypted.cpp \
    src/core/encryption/qaesencryption.cpp \
    src/core/kcdb.cpp \
    src/core/main.cpp

HEADERS += \
    src/core/keymapjsonengine.h \
    src/core/commoninclude.h \
    src/core/debugshowoptions.h \
    src/core/encryption/encrypted.h \
    src/core/encryption/qaesencryption.h \
    src/core/kcdb.h

RESOURCES += \
    src/resource/config.qrc \
    src/resource/pic.qrc \
    src/resource/stylesheet.qrc

DISTFILES += \
    README.md \
    log \
    src/android/AndroidManifest.xml \
    src/android/build.gradle \
    src/android/gradle.properties \
    src/android/gradle/wrapper/gradle-wrapper.jar \
    src/android/gradle/wrapper/gradle-wrapper.properties \
    src/android/gradlew \
    src/android/gradlew.bat \
    src/android/res/values/libs.xml

# windows config
win32 {
    TARGET = "Key Container"
    LIBS +=  "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\User32.Lib"
    CONFIG(debug,debug|release){
        INCLUDEPATH += C:\VisualLeakDetector\include
        DEPENDPATH += C:\VisualLeakDetector\include
        if(contains(DEFINES, COMPILE_VID)) {
            LIBS += -LC:/VisualLeakDetector/lib/Win64 -lvld
            DEFINES += ENABLE_VID
        }

    }
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
    SOURCES += src/core/qmlimporter.cpp
    HEADERS += src/core/qmlimporter.h
    RESOURCES += src/qml/qml.qrc
    DEFINES += ENABLE_QML
}
else {
    # Wigets config
    QT += gui widgets
    SOURCES += \
        src/utils/animationrefresh.cpp \
        src/gui/findkeyui.cpp \
        src/gui/inputinitkeyui.cpp \
        src/gui/inputkeyui.cpp \
        src/gui/login.cpp \
        src/gui/mainui.cpp \
        src/utils/qssinstaller.cpp \
        src/utils/widget/messageboxexx.cpp \
        src/utils/widget/qlineeditpro.cpp \
        src/utils/widget/tablewidgetex.cpp \
        src/utils/widget/titlebar.cpp \
        src/utils/style/proxystyle.cpp

    HEADERS += \
        src/utils/animationrefresh.h \
        src/gui/findkeyui.h \
        src/gui/inputinitkeyui.h \
        src/gui/inputkeyui.h \
        src/gui/login.h \
        src/gui/mainui.h \
        src/utils/qssinstaller.h \
        src/utils/widget/messageboxexx.h \
        src/utils/widget/qlineeditpro.h \
        src/utils/widget/tablewidgetex.h \
        src/utils/widget/titlebar.h \
        src/utils/style/proxystyle.h

    FORMS += \
        src/gui/findkeyui.ui \
        src/gui/inputinitkeyui.ui \
        src/gui/inputkeyui.ui \
        src/gui/login.ui \
        src/gui/mainui.ui \
        src/utils/widget/messageboxexx.ui
}

android:{
    QT += androidextras
}

win32-msvc* {
  QMAKE_CXXFLAGS += /utf-8
# use -j N in project settings when using jom instead
#  QMAKE_CXXFLAGS += /MP
}

win32{
    CONFIG(debug,debug|release){
        instll_login_ec.path = $$OUT_PWD/debug
    }
    else{
        instll_login_ec.path = $$OUT_PWD/release
    }
}
unix:!android{
    instll_login_ec.path = $$OUT_PWD
}
instll_login_ec.files += $$PWD/src/resource/config/login.ec
INSTALLS += instll_login_ec

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

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/src/android

