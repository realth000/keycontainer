#ifndef DEBUGSHOWOPTIONS_H
#define DEBUGSHOWOPTIONS_H
#include <QDebug>

//#define DEBUG_ALL_CONTROL_CLOSE


//#define DEBUG_SHOW_KEYS
//#define DEBUG_SKIP_LOGIN
//#define DEBUG_QML_SHOW_INFO
//#define DEBUG_SHOW_IO_PATH
//#define DEBUG_DISABLE_SINGLE_DETECTION

// 此处DEBUG_QML_ON_WINDOWS勿动，使用.pro文件来控制是否编译qml版
#ifdef ENABLE_QML
#define DEBUG_QML_ON_WINDOWS
#endif

#ifdef Q_OS_ANDROID
#define ANDROID_PACKAGE_NAME "com.th000.keycontainer"
//#define ANDROID_PRIVATE_STORAGE
#endif

#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#define DEBUG_QML_PASS_PASSWORD
#endif

#ifdef DEBUG_ALL_CONTROL_CLOSE
#ifdef DEBUG_SHOW_KEYS
#undef DEBUG_SHOW_KEYS
#endif
#ifdef DEBUG_SKIP_LOGIN
#undef DEBUG_SKIP_LOGIN
#endif
#ifdef DEBUG_QML_PASS_PASSWORD
#undef DEBUG_QML_PASS_PASSWORD
#endif
#endif



class debugShowOptions
{
public:
    debugShowOptions();
};

#endif // DEBUGSHOWOPTIONS_H
