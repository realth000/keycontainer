#include <iostream>
#include "encryption/encrypted.h"
#include <QDebug>
#include <QTime>
#include <type_traits>
#include <QRandomGenerator>
#include <QRandomGenerator64>
#include "debugshowoptions.h"

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

using std::is_same;

template<typename Type>
EncryptedClass<Type>::EncryptedClass(int initType)
{
    initKey(initType);
    this->initType = initType;
}

template<typename Type>
Type EncryptedClass<Type>::EncryptVal(Type val)
{
    return initKeyFlag ? DoVal(val) : 0;
}

template<typename Type>
Type EncryptedClass<Type>::EncryptCheck(Type check)
{
    return initKeyFlag ? DoCheck(check) : 0;
}

template<typename Type>
Type EncryptedClass<Type>::Decrypt(Type val, Type check) const
{
//    qDebug()<< "ttttttttttttttttttttttt" << check << key_check_64;
    if constexpr(is_same<Type, QString>::value){
        Type tv, tc;
        tv = QXorKeyVal(val);
        tc = QXorKeyCheck(check);
        if(tv == tc){
            return tc;
        }
        else{
//            qDebug() << tv <<"   s   "<< tc;
            throw MemoryCheatException();
        }
    }
    else{
        return Undo(val, check);
    }
}

template<typename Type>
void EncryptedClass<Type>::initKey(int initType)
{
    // when qsrand() and qrand() are depred, use QRandomGenerator
    // TODO: Test QRandomGenerator on GCC + Qt5.12.10. If work, totally use QRandomGenerator instead.
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
//    qDebug() << "明天去哪吃? [1-3]" << QRandomGenerator::securelySeeded().bounded(1,4);
    switch (initType) {

    case 1:
        key_16 = QRandomGenerator::securelySeeded().bounded(0x1000, 0x6FFF);
        key_check_16 = QRandomGenerator::securelySeeded().bounded(0x1000, 0x6FFF);
#ifdef DEBUG_SHOW_KEYS
                qDebug() << "init key_16 and key_check_16:" << key_16 << key_check_16;
#endif
        break;

    case 2:
        key_32 = QRandomGenerator::securelySeeded().bounded(0x10000000, 0x6FFFFFFF);
        key_check_32 = QRandomGenerator::securelySeeded().bounded(0x10000000, 0x6FFFFFFF);
#ifdef DEBUG_SHOW_KEYS
                qDebug() << "init key_32 and key_check_32:" << key_32 << key_check_32;
#endif
        break;

    case 3:
//         Control the value of key_64 and key_check_64 valid for quint64.
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        while (key_64 <= 0x1000000000000000 || key_check_64 <= 0x1000000000000000
               || key_64 >= 0x6FFFFFFFFFFFFFFF || key_check_64 >= 0x6FFFFFFFFFFFFFFF) {
            key_64 =  QRandomGenerator64::securelySeeded().generate64();
            key_check_64 =  QRandomGenerator64::securelySeeded().generate64();
        }
#ifdef DEBUG_SHOW_KEYS
                qDebug() << "init key_64 and key_check_64:" << key_64 << key_check_64;
#endif
        break;
    case 4:
        key_string = QRandomGenerator::securelySeeded().bounded(0x1000, 0x6FFF);
        key_check_string = QRandomGenerator::securelySeeded().bounded(0x1000, 0x6FFF);
#ifdef DEBUG_SHOW_KEYS
        qDebug() << "init key_string and key_check_string" << key_string << key_check_string;
#endif
        break;

    default:
        return;
    }
#else
    switch (initType) {
    case 1:
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        key_16 = 0x1000 + qrand() % (0x7FFF - 0x1000);
        key_check_16 = 0x1000 + qrand() % (0x7FFF - 0x1000);
        //        qDebug() << "init key_16 and key_check_16:" << key_16 << key_check_16;
        break;

    case 2:
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        key_32 = 0x10000000 + qrand() % (0x7FFFFFFF - 0x10000000);
        key_check_32 = 0x10000000 + qrand() % (0x7FFFFFFF - 0x10000000);
        //        qDebug() << "init key_32 and key_check_32:" << key_32 << key_check_32;
        break;

    case 3:
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        key_64 = 0x1000000000000000 + qrand() % (0x7FFFFFFFFFFFFFFF - 0x1000000000000000);
        key_check_64 = 0x1000000000000000 + qrand() % (0x7FFFFFFFFFFFFFFF - 0x1000000000000000);
        //        qDebug() << "init key_64 and key_check_64:" << key_64 << key_check_64;
        break;
    case 4:
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        key_string = 0x1000 + qrand() % (0x7FFF - 0x1000);
        key_check_string = 0x1000 + qrand() % (0x7FFF - 0x1000);
        //        qDebug() << "init key_string and key_check_string" << key_string << key_check_string;
        break;
    default:
        return;
    }

#endif

    initKeyFlag = true;
}

template<typename Type>
Type EncryptedClass<Type>::QXorKeyVal(Type val) const
{
    if constexpr(is_same<Type, QString>::value){
        int const len = val.length();
        QString tQString = val;
        QChar* tQChar = tQString.data();
//        Q_UNUSED(*tQChar);
        for(int i=0; i<len; i++){

            tQChar[i] = tQChar[i].unicode() ^ key_string;
        }
        // deep copy? TODO:
        QString result = QString(tQChar);
//        delete [] tQChar;
//        qDebug() << "QXorKeyVal result:" << result;
        return result;
    }
    else{
        return 0;
    }
}

template<typename Type>
Type EncryptedClass<Type>::QXorKeyCheck(Type check) const
{
    if constexpr(is_same<Type, QString>::value){
        int const len = check.length();
        QString tQString = check;

        // TODO: On MSVC2017 and MinGW:
        // HEAP[test.exe]: HEAP: Free Heap block 000001FF35AD4240 modified at 000001FF35AD4298 after it was freed
        QChar* tQChar = tQString.data();
//        Q_UNUSED(*tQChar);
        for(int i=0; i<len; i++){
#if _MSC_VER >= 1600
            tQChar[i] = tQChar[i].unicode() ^ key_check_string;
#else
            tQChar[i] = tQChar[i].unicode() ^ key_check_16;
#endif
        }

        // TODO: deep copy?
        QString result = QString(tQChar);
//        qDebug() << "QXorKeyCheck result:" << result;
        return result;
    }
    else{
        return 0;
    }

}

template<typename Type>
bool EncryptedClass<Type>::QXorKey(Type val, Type check)
{
    int const len1 = val.length();
    int const len2 = check.length();
    if(len1 != len2){
        throw MemoryCheatException();
    }
    QString tQString1 = val;
    QString tQString2 = val;
    QChar* tQChar1 = tQString1.data();
    QChar* tQChar2 = tQString2.data();
//    Q_UNUSED(*tQChar1);
//    Q_UNUSED(*tQChar2);
    for(int i=0; i<len1; i++){
#if _MSC_VER >= 1600
        tQChar1[i] = tQChar1[i].unicode() ^ key_string;
        tQChar2[i] = tQChar2[i].unicode() ^ key_string;
#else
        tQChar1[i] = tQChar1[i].unicode() ^ key_string;
        tQChar2[i] = tQChar2[i].unicode() ^ key_string;
#endif
    }

    if(QString(tQChar1)== QString(tQChar2)){
        delete [] tQChar1;
        delete [] tQChar2;
        return true;
    }
    else{
        delete [] tQChar1;
        delete [] tQChar2;
        return false;
    }
}

template<typename Type>
Type EncryptedClass<Type>::DoVal(Type val)
{
    if constexpr(is_same<Type, quint16>::value){
        return val ^ key_16;
    }
    else if constexpr(is_same<Type, quint32>::value){
        return val ^ key_32;
    }
    else if constexpr(is_same<Type, quint64>::value){
//        qDebug() << "DoVal64" << val << key_64 << static_cast<quint64>(val ^ key_64);
        return val ^ key_64;
    }
    else if constexpr(is_same<Type, QString>::value){
        return QXorKeyVal(val);
    }
    else{
        return 0;
    }
}

template<typename Type>
Type EncryptedClass<Type>::DoCheck(Type check)
{
    if constexpr(is_same<Type, quint16>::value){
        return check ^ key_check_16;
    }
    else if constexpr(is_same<Type, quint32>::value){
        return check ^ key_check_32;
    }
    else if constexpr(is_same<Type, quint64>::value){
//        qDebug() << "DoCheck64" << check << key_check_64 << static_cast<quint64>(check ^ key_check_64);
        return check ^ key_check_64;
    }
    else if constexpr(is_same<Type, QString>::value){
        return QXorKeyCheck(check);
    }
    else{
        return 0;
    }
}

template<typename Type>
Type EncryptedClass<Type>::Undo(Type val, Type check) const
{
    if constexpr(is_same<Type, quint16>::value){
        if((val ^ key_16) == (check ^ key_check_16)){
            return val ^ key_16;
        }
        else{
            throw MemoryCheatException();
        }
    }
    else if constexpr(is_same<Type, quint32>::value){
        if((val ^ key_32) == (check ^ key_check_32)){
            return val ^ key_32;
        }
        else{
            throw MemoryCheatException();
        }
    }
    else if constexpr(is_same<Type, quint64>::value){
        if((val ^ key_64) == (check ^ key_check_64)){
            return val ^ key_64;
        }
        else{
//            qDebug() << "about to Exception" << val << check << key_64 << key_check_64;
            throw MemoryCheatException();
        }
    }
    else if constexpr(is_same<Type, QString>::value){
        if constexpr(QXorKey(val, check)){
            return QXorKey(val);
        }
        else{
            throw MemoryCheatException();
        }
    }
    else{
        return 0;
    }
}

Euint_16::Euint_16()
{

}

Euint_16::Euint_16(quint16 val)
{
    setVal(val);
}

quint16 Euint_16::getVal() const
{
    return this->ec_16.Decrypt(this->val, this->check);
}

void Euint_16::setVal(quint16 val)
{
    this->ec_16 = EncryptedClass<quint16>(1);
    this->val = ec_16.EncryptVal(val);
//    qDebug() <<val;
    this->check = ec_16.EncryptCheck(val);
}

Euint_32::Euint_32()
{

}

Euint_32::Euint_32(quint32 val)
{
    setVal(val);
}

quint32 Euint_32::getVal() const
{
    return this->ec_32.Decrypt(this->val, this->check);
}

void Euint_32::setVal(quint32 val)
{
    this->ec_32 = EncryptedClass<quint32>(2);
    this->val = ec_32.EncryptVal(val);
    this->check = ec_32.EncryptCheck(val);
}


Euint_64::Euint_64()
{

}

Euint_64::Euint_64(quint64 val)
{
    setVal(val);
//    qDebug() << "64 after encrypt "<<this->val << this->check;
}

quint64 Euint_64::getVal() const
{
//    qDebug() <<"64: before decrypt" << this->val << this->check;
    return this->ec_64.Decrypt(this->val, this->check);
}

void Euint_64::setVal(quint64 val)
{
    this->ec_64 = EncryptedClass<quint64>(3);
    this->val = ec_64.EncryptVal(val);
    this->check = ec_64.EncryptCheck(val);
}

Estring::Estring()
{

}

Estring::Estring(QString val)
{
    setVal(val);
//    qDebug() << "string: after decrypt" << val<<this->val << this->check;
}

QString Estring::getVal() const
{
    return this->ec_s.Decrypt(this->val, this->check);
}

void Estring::setVal(QString val)
{
    this->ec_s = EncryptedClass<QString>(4);
    this->val = ec_s.EncryptVal(val);
    this->check = ec_s.EncryptCheck(val);
}
