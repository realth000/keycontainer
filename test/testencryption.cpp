#include "testencryption.h"
#include <QtCore/QRandomGenerator>
#include <QtCore/QRandomGenerator64>
#include <QtTest/QTest>

#define private public
#include "encryption/encrypted.h"
#undef private

TestEncryption::TestEncryption(QObject *parent) : QObject(parent)
{

}

void TestEncryption::testBasicEncryption()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    const quint16 E = QRandomGenerator::securelySeeded().bounded(0x1000, 0x6FFF);;
#else
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    const quint16 E = 0x1000 + qrand() % (0x7FFF - 0x1000);
#endif
    EncryptedClass<quint16> t16(1);
    QVERIFY(t16.initKeyFlag == true);
    QVERIFY(t16.initType == 1);
    QVERIFY(t16.key_16 != 0);
    QVERIFY(t16.key_check_16 != 0);
    QVERIFY(t16.key_16 != t16.key_check_16);
    const quint16 val = t16.EncryptVal(E);
    const quint16 check = t16.EncryptCheck(E);
    QVERIFY(val != check);
    QVERIFY(E == t16.Decrypt(val, check));
}

void TestEncryption::testEuint_16()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    const quint16 E = QRandomGenerator::securelySeeded().bounded(0x1000, 0x6FFF);;
#else
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    const quint16 E = 0x1000 + qrand() % (0x7FFF - 0x1000);
#endif
    Euint_16 t1(E);
    QVERIFY(t1.ec_16.initKeyFlag == true);
    QVERIFY(E == t1.getVal());
    Euint_16 t2;
    t2.setVal(E);
    QVERIFY(t2.ec_16.initKeyFlag == true);
    QVERIFY(E == t2.getVal());
}

void TestEncryption::testEuint_32()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    const quint32 E = QRandomGenerator::securelySeeded().bounded(0x10000000, 0x6FFFFFFF);;
#else
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    const quint16 E = 0x10000000 + qrand() % (0x7FFFFFFF - 0x10000000);
#endif
    Euint_32 t1(E);
    QVERIFY(t1.ec_32.initKeyFlag == true);
    QVERIFY(E == t1.getVal());
    Euint_32 t2;
    t2.setVal(E);
    QVERIFY(t1.ec_32.initKeyFlag == true);
    QVERIFY(E == t2.getVal());
}

void TestEncryption::testEuint_64()
{
    quint64 E(0);
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    while (E <= 0x1000000000000000 || E >= 0x6FFFFFFFFFFFFFFF) {
        E = QRandomGenerator64::securelySeeded().generate64();
    }
#else
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    E = 0x1000000000000000 + qrand() % (0x7FFFFFFFFFFFFFFF - 0x1000000000000000);
#endif
    Euint_64 t1(E);
    QVERIFY(t1.ec_64.initKeyFlag == true);
    QVERIFY(E == t1.getVal());
    Euint_64 t2;
    t2.setVal(E);
    QVERIFY(t1.ec_64.initKeyFlag == true);
    QVERIFY(E == t2.getVal());
}

void TestEncryption::testEstring()
{
    QString E("aA_*\\|23");
    Estring t1(E);
    QVERIFY(t1.ec_s.initKeyFlag == true);
    QVERIFY(E == t1.getVal());
    Estring t2;
    t2.setVal(E);
    QVERIFY(t1.ec_s.initKeyFlag == true);
    QVERIFY(E == t2.getVal());
}
