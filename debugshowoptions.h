#ifndef DEBUGSHOWOPTIONS_H
#define DEBUGSHOWOPTIONS_H
#include <QDebug>

//#define DEBUG_SHOW_KEYS
//#define DEBUG_SKIP_LOGIN
//#define DEBUG_QML_SHOW_INFO

// 此处DEBUG_QML_ON_WINDOWS勿动，使用.pro文件来控制是否编译qml版
#ifdef ENABLE_QML
#define DEBUG_QML_ON_WINDOWS
#endif

#ifdef Q_OS_ANDROID
#define ANDROID_PACKAGE_NAME "com.th000.keycontainer"
//#define ANDROID_PRIVATE_STORAGE
#endif




class debugShowOptions
{
public:
    debugShowOptions();
};

#endif // DEBUGSHOWOPTIONS_H
