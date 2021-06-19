#ifndef KEYMAPJSONENGINE_H
#define KEYMAPJSONENGINE_H

#include "../commoninclude.h"

class KeyMapJsonEngine
{
public:
    KeyMapJsonEngine();
    static QString keyMapToJson(QMap<int, KeyMap> keyMap);
    static QString keyMapToJson(QMap<int, KeyMap> keyMap, QList<bool> select);
    static QMap<int, KeyMap> jsonToKeyMap(QString json);
};

#endif // KEYMAPJSONENGINE_H
