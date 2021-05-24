#include "debugshowoptions.h"

debugShowOptions::debugShowOptions()
{
#ifdef DEBUG_SHOW_KEYS
    qDebug() << "DebugShowOptions: DEBUG_SHOW_KEYS enabled";
#endif

#ifdef DEBUG_SKIP_LOGIN
    qDebug() << "DebugShowOptions: DEBUG_SKIP_LOGIN enabled";
#endif
}
