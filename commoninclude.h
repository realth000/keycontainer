#ifndef COMMONINCLUDE_H
#define COMMONINCLUDE_H

#include <QWidget>
#include "encryption/encrypted.h"

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
