#ifndef TEST_H
#define TEST_H
#include <QtGlobal>
#include <QString>
#include "debugshowoptions.h"

#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#include <QMetaType>
#endif

class MemoryCheatException
{

};

template<typename Type>
class EncryptedClass{
public:
    EncryptedClass(){};
    explicit EncryptedClass(int initType);
    Type EncryptVal(Type val);
    Type EncryptCheck(Type check);
    Type Decrypt(Type val, Type check) const;

private:
    void initKey(int initType);
    Type QXorKeyVal(Type val) const;
    Type QXorKeyCheck(Type val) const;
    bool QXorKey(Type val, Type check);
    Type DoVal(Type val);
    Type DoCheck(Type check);
    Type Undo(Type val, Type check) const;

    bool initKeyFlag = false;
    int initType = -1;
    quint16 key_16 = 0x1000;
    quint16 key_check_16 = 0x1000;
    quint32 key_32 = 0x10000000;
    quint32 key_check_32 = 0x10000000;
    quint64 key_64 = 0x1000000000000000;
    quint64 key_check_64 = 0x1000000000000000;
    quint16 key_string = 0x1000;
    quint16 key_check_string = 0x1000;
};

class Euint_16{
public:
    Euint_16();
    explicit Euint_16(quint16 val);
    quint16 getVal() const;
    void setVal(quint16 val);

private:
    EncryptedClass<quint16> ec_16;
    quint16 val;
    quint16 check;
};

class Euint_32{
public:
    Euint_32();
    explicit Euint_32(quint32 val);
    quint32 getVal() const;
    void setVal(quint32 val);

private:
    EncryptedClass<quint32> ec_32;
    quint32 val;
    quint32 check;
};

class Euint_64{
public:
    Euint_64();
    explicit Euint_64(quint64 val);
    quint64 getVal() const;
    void setVal(quint64 val);

private:
    EncryptedClass<quint64> ec_64;
    quint64 val;
    quint64 check;
};

class Estring{
public:
    Estring();
    explicit Estring(QString val);
    QString getVal() const;
    void setVal(QString val);

private:
    EncryptedClass<QString> ec_s;
    QString val;
    QString check;
};



#endif // TEST_H
