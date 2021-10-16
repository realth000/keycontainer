#ifndef COMMONINCLUDE_H
#define COMMONINCLUDE_H

//#include <QWidget>
#include "encryption/encrypted.h"
#include <QDateTime>

#define TO_STR1(I1) #I1
#define TO_STR2(I2) TO_STR1(I2)

#define ABOUT_VERSION APP_VERSION
#define ABOUT_TIME  "2021.10.16"
#define ABOUT_BASE_QT "Qt " QT_VERSION_STR
#define ABOUT_BASE_CPP __cplusplus

#ifdef Q_CC_MSVC
#   define ABOUT_BASE_COMPILER_TYPE "MSVC "
#   define ABOUT_BASE_COMPILER _MSC_VER
#elif defined(Q_CC_GNU)
#   ifdef Q_CC_CLANG
#   define ABOUT_BASE_COMPILER_TYPE "GCC "
#   define ABOUT_BASE_COMPILER TO_STR2(__GNUC__) "." TO_STR2(__GNUC_MINOR__) "." TO_STR2(__GNUC_PATCHLEVEL__) "\n"\
    "Clang " TO_STR2(__clang_major__) "." TO_STR2(__clang_minor__)
#   define ABOUT_BASE_COMPILER_STRING
#else
#   define ABOUT_BASE_COMPILER_TYPE "GCC "
#   define ABOUT_BASE_COMPILER TO_STR2(__GNUC__) "." TO_STR2(__GNUC_MINOR__) "." TO_STR2(__GNUC_PATCHLEVEL__)
#   define ABOUT_BASE_COMPILER_STRING
#   endif
#elif defined(Q_CC_CLANG)
#   define ABOUT_BASE_COMPILER_TYPE "CLANG "
#   define ABOUT_BASE_COMPILER TO_STR2(__clang_major__) "." TO_STR2(__clang_minor__)
#   define ABOUT_BASE_COMPILER_STRING
#else
#   define ABOUT_BASE_COMPILER_TYPE "Unknown "
#   define ABOUT_BASE_COMPILER "  -  "
#endif

#if defined(Q_OS_LINUX)  && !defined(Q_OS_ANDROID)
#   define ABOUT_PLANTFORM "For Linux"
#   define TITLEBAR_TITLEICON ":/src/KeyContainer.ico"
#   define TITLEBAR_TITLETEXT "KeyContainer"
#elif defined(Q_OS_WINDOWS)
#   define ABOUT_PLANTFORM "For Windows"
#   define TITLEBAR_TITLEICON ":/src/Key Container.ico"
#   define TITLEBAR_TITLETEXT "Key Container"
#elif defined (Q_OS_ANDROID)
#   define ABOUT_PLANTFORM "For Android"
#else
#   define ABOUT_PLANTFORM "Unknown"
#   define TITLEBAR_TITLEICON ":/src/KeyContainer.ico"
#   define TITLEBAR_TITLETEXT "KeyContainer"
#endif

#define TITLEBAR_CLOSEICON ":/src/close.png"


// ProxyStyle defines

#define TABBAR_LABEL_WIDTH             120
#define TABBAR_LABEL_HEIGHT             60
#define TABBAR_SHADOW_WIDTH              3
#define TABWIDGET_BUTTON_UP_HEIGHT      43                      // tabwidget menu的高度
#define TABBAR_BACKGROUND_COLOR               "#232323"     // tabwidget左侧选项卡的label的背景颜色，需要和qss里QTabWidget的背景颜色一致
#define TABWIDGET_MENU_BACKGROUND_COLOR       "#282828"     // 一横条的背景，宽度是button_up_height + offset
#define PUSHBUTTON_DISABLE_COLOR              "#282828"     // disable color 要比上边这个menu background color 浅
#define PUSHBUTTON_ON_WIDGET_BACKGROUND_COLOR "#333333"     // 设置在widget上而不在一横条背景上的PushButton在普通状态时的背景颜色
#define CHECKBOX_DIABLE_COLOR                 "#808080"     // QCheckBox disabled color
#define CHECKBOX_CHECK_WIDTH             4

#define TABWIDGET_MENU_HORIZONTAL_MARGIN 0     // 一横条的高与pushbutton的高度差，要比pushbutton宽。
#define TABWIDGET_MENU_VERTICAL_MARGIN   5     // 一横条的高与pushbutton的高度差，要比pushbutton宽。

#define PADDING_UP    4
#define PADDING_LEFT  4
#define PADDING_DOWN  4

#define KEYMAP_JSON_ENGINE_VERSION "1.0"

// password defines

#define LOGIN_PASSWD_FILE_NAME "/login.ec"
#define LOGIN_PASSWD_HASH_DEF  "24972516c095bc4c5c916c9eb9c6b01d99939bcc600b8261bbb63df1baf56289ac292f847a9b28141ca91b5c4a26f4e799df6eeef5547df09d00d8c5f1d857e2"
#define KEYDB_PASSWD_FILE_NAME "/dat.ec"
#define DEFAULT_LOCK_APP_TIME  "5分钟" // 要保证timeLockerTimingMap里有DEFAULT_LOCK_APP_TIME
#define DEFAULT_STR_GEN_LENGTH 6
#define PASSWD_FILE_FORM_ALL   "全部格式 (*.kcdb *.kcdj)"
#define PASSWD_FILE_FORM_KCDB  "KeyContainerDataBase (*.kcdb)"
#define PASSWD_FILE_FORM_JSON  "KeyContainerDataJson (*.kcdj)"

namespace CI{
    class KeyMap;
}

class KeyMap{

public:
    KeyMap();
    KeyMap(quint32 id, Estring disc, Estring account, Estring password);
    ~KeyMap();
    quint32 id;
    Estring disc;
    Estring account;
    Estring password;

};
Q_DECLARE_METATYPE(KeyMap);

#endif // COMMONINCLUDE_H
