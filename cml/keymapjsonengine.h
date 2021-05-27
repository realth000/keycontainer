#ifndef KEYMAPJSONENGINE_H
#define KEYMAPJSONENGINE_H

#include "../commoninclude.h"

class KeyMapJsonEngine
{
public:
    KeyMapJsonEngine();
    static QString keyMapToJson(QMap<int, KeyMap> keyMap);
};

#endif // KEYMAPJSONENGINE_H
