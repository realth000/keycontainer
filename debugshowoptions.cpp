#include "debugshowoptions.h"

DebugShowOptions::DebugShowOptions()
{
#ifdef DEBUG_SHOW_KEYS
    qDebug() << "DebugShowOptions: DEBUG_SHOW_KEYS enabled";
#endif

#ifdef DEBUG_SKIP_LOGIN
    qDebug() << "DebugShowOptions: DEBUG_SKIP_LOGIN enabled";
#endif

#ifdef DEBUG_QML_ON_WINDOWS
    qDebug() << "DebugShowOptions: DEBUG_QML_ON_WINDOWS enabled";
#endif

#ifdef DEBUG_QML_SHOW_INFO
    qDebug() << "DebugShowOptions: DEBUG_QML_SHOW_INFO enabled";
#endif

#ifdef ANDROID_PRIVATE_STORAGE
    qDebug() << "DebugShowOptions: ANDROID_PRIVATE_STORAGE enabled";
#endif

#ifdef DEBUG_ALL_CONTROL_CLOSE
    qDebug() << "DebugShowOptions: DEBUG_ALL_CONTROL_CLOSE enabled";
#endif

#ifdef DEBUG_DISABLE_SINGLE_DETECTION
    qDebug() << "DebugShowOptions: DEBUG_DISABLE_SINGLE_DETECTION enabled";
#endif
}
