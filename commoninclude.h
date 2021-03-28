#ifndef COMMONINCLUDE_H
#define COMMONINCLUDE_H

#include <QWidget>
#include "encryption/encrypted.h"

#define TO_STR1(I1) #I1
#define TO_STR2(I2) TO_STR1(I2)

#define ABOUT_VERSION "2.1.2"
#define ABOUT_TIME "2021-3-28"
#define ABOUT_BASE_QT "Qt " QT_VERSION_STR


#ifdef Q_CC_MSVC
#   define ABOUT_BASE_COMPILER "MSVC " TO_STR2(_MSVC_LANG)
#elif defined(Q_CC_MINGW)
#   define ABOUT_BASE_COMPILER "MinGW " TO_STR2(__GNUC__) "." TO_STR2(__GNUC_MINOR__) "." TO_STR2(__GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
#   define ABOUT_BASE_COMPILER "GCC " TO_STR2(__GNUC__) "." TO_STR2(__GNUC_MINOR__) "." TO_STR2(__GNUC_PATCHLEVEL__)
#else
#   define ABOUT_BASE_COMPILER "Unknown"
#endif

#ifdef Q_OS_LINUX
#   define ABOUT_PLANTFORM "For Linux"
#elif defined Q_OS_WINDOWS
#   define ABOUT_PLANTFORM "For Windows"
#else
#   define ABOUT_PLANTFORM "Unknown"
#endif


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
