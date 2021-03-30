#ifndef KCDB_H
#define KCDB_H
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QMap>
#include "encryption/encrypted.h"

#define NEXT_IS_GROUPKEY 0x01
#define NEXT_IS_MAINTAINKEY 0x02
#define END_OF_KCDB 0x03
#define BEGIN_OF_GROUPLEY 0x10
#define BEGIN_OF_MTKEY 0x11

class kcdbFileDamagedException{

};

class EOF_Fail_Expection{

};

class AesClass : public QObject
{
    Q_OBJECT

public slots:
//    friend class kcdb;
    void initTestCase(QString key_in);
    QByteArray CFB256Encrypt(QString inputStr);
    QString CFB256Decrypt(QByteArray inputStr);

private:
    QByteArray key32;
    QByteArray iv;
    QByteArray in;
    QByteArray inCBC128;
    QByteArray outCBC128;
    QString aesKey ;
    QByteArray hashKey;

};
class Kcdb_io
{
public:
    Kcdb_io();

protected:
    void output(QDataStream &outStream, Estring data, AesClass *code);
    void output(QDataStream &outStream, quint8 argc, AesClass *code);
    void outKcdbHead(QDataStream &outStream,quint8 inType, AesClass *code);
    void outKcdb(QDataStream &outStream, Estring disc, Estring keyNameOrLevel, Estring iKey, Estring iCheckKey, quint8 EOF_, AesClass *code);
    void input(QDataStream &inStream, QString &data, AesClass *code);
    void input(QDataStream &inStream, quint8 &argc, AesClass *code);
    quint8 readKcdbHead(QDataStream &inStream, AesClass *code);
    QStringList readKeys(QDataStream &inStream, AesClass *code);
    QList<QList<QStringList>> inKcdb(QDataStream &inStream, AesClass *code);

private:
//    QFile inFile;
//    QFile outFile;
//    QDataStream inStream;
//    QDataStream outStream;
//    bool backupState = false;
};

class GroupKey{
public:
    explicit GroupKey(Estring disc, Estring account, Estring keyValue);
    explicit GroupKey(QString disc, QString account, QString keyValue);
    QString getiKeyValue() const;
    QString getiCheckKeyValue() const;
    Estring getiKey() const;
    Estring getiCheckKey() const;

    Estring disc;
    Estring account;
    Estring key;
    Estring checkKey;

private:
    // TODO:needed ?
    Estring iKey;
    Estring iCheckKey;
};

class Kcdb : public Kcdb_io
{
public:
    explicit Kcdb(QString workPath);
    void changeBackupState(bool state);
    void inserKey(QString disc, GroupKey key);
    void clearKeys();
    bool readKcdb();
    void writeKcdb();
    void setKeys(QMap<QString, GroupKey> keyList);
    QMap<QString, GroupKey> getKeys() const;
    Estring getKey() const;
    Estring getKey_in() const;
    Estring getAESKeyPath() const;
    void setKey(QString k);
    void setKey_in(QString k);
    void setBackupState(bool isBackup);
    void setSavePath(QString path);
    void setBackupPath(QString path);
    QFile inFile;
    QFile outFile;

private:
    Kcdb_io ko;
    QString workPath="";
    Estring key = Estring("259741592652");
    Estring key_in = Estring("259741592652");
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    QString aesPath = "/dat.ec";
    QDataStream inStream;
    QDataStream outStream;
    // if backup, true; if save, false;
    bool backupState = false;

    void showAllKey();

    QMap<QString, GroupKey> groupKeyList;
    QMutableMapIterator<QString, GroupKey> gkptr = groupKeyList;
//    QMutableMapIterator<int, maintainKey>kcdb::mtkptr = maintainKey::mtKeyList;

};

#endif // KCDB_H
