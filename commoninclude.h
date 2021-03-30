#ifndef COMMONINCLUDE_H
#define COMMONINCLUDE_H

#include <QWidget>
#include "encryption/encrypted.h"

#define TO_STR1(I1) #I1
#define TO_STR2(I2) TO_STR1(I2)

#define ABOUT_VERSION "2.1.4"
#define ABOUT_TIME "2021-3-30"
#define ABOUT_BASE_QT "Qt " QT_VERSION_STR
#define ABOUT_BASE_CPP __cplusplus

#ifdef Q_CC_MSVC
#   define ABOUT_BASE_COMPILER_TYPE "MSVC "
#   define ABOUT_BASE_COMPILER _MSVC_LANG
#elif defined(Q_CC_MINGW)
#   define ABOUT_BASE_COMPILER_TYPE "MinGW"
#   define ABOUT_BASE_COMPILER "MinGW " TO_STR2(__GNUC__) "." TO_STR2(__GNUC_MINOR__) "." TO_STR2(__GNUC_PATCHLEVEL__)
#   define ABOUT_BASE_COMPILER_STRING
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

#ifdef Q_OS_LINUX
#   define ABOUT_PLANTFORM "For Linux"
#elif defined Q_OS_WINDOWS
#   define ABOUT_PLANTFORM "For Windows"
#else
#   define ABOUT_PLANTFORM "Unknown"
#endif

#define TITLEBAR_TITLEICON ":/src/Key Container.ico"
#define TITLEBAR_CLOSEICON ":/src/close.png"

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
