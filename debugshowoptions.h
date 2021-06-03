#ifndef DEBUGSHOWOPTIONS_H
#define DEBUGSHOWOPTIONS_H
#include <QDebug>

//#define DEBUG_SHOW_KEYS
#define DEBUG_SKIP_LOGIN
//#define DEBUG_QML_SHOW_INFO

// 此处DEBUG_QML_ON_WINDOWS勿动，使用.pro文件来控制是否编译qml版
#ifdef ENABLE_QML
#define DEBUG_QML_ON_WINDOWS
#endif




class debugShowOptions
{
public:
    debugShowOptions();
};

#endif // DEBUGSHOWOPTIONS_H
