#ifndef KCDB_H
#define KCDB_H

#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QMap>
#include "encryption/encrypted.h"
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#else
#include <QWidget>
#include "widget/messageboxexx.h"
#endif
#define KCDB_ENGINE_VERSION 0x00000010
#define KCDB_WRITE_KEY_CONTINUE 0x00000100
#define KCDB_FILE_END 0x10000000

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
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
class Kcdb_io: public QObject
{
    Q_OBJECT
#else
class Kcdb_io
{
#endif
public:
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
signals:
    void qml_msg_info(QString msg);
#endif

protected:
    void output(QDataStream &outStream, Estring data, AesClass *AESMachine);
    void output(QDataStream &outStream, int argc, AesClass *AESMachine);
    void outKcdbHead(QDataStream &outStream,int data, AesClass *AESMachine);
    void outKcdb(QDataStream &outStream, Estring disc, Estring account, Estring iKey, Estring iCheckKey, int kcdb_pos, AesClass *AESMachine);
    void input(QDataStream &inStream, QString &data, AesClass *AESMachine);
    void input(QDataStream &inStream, int &argc, AesClass *AESMachine);
    int readKcdbHead(QDataStream &inStream, AesClass *AESMachine);
    QStringList readKeys(QDataStream &inStream, AesClass *AESMachine);
    QList<QList<QStringList>> inKcdb(QDataStream &inStream, AesClass *AESMachine);

private:
//    QFile inFile;
//    QFile outFile;
//    QDataStream inStream;
//    QDataStream outStream;
//    bool backupState = false;
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#else
    MessageBoxExX t;
#endif

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

#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
class Kcdb : public Kcdb_io
{
    Q_OBJECT
#else
class Kcdb : public Kcdb_io, QWidget
{
#endif
public:
    explicit Kcdb(QString savePath, QString backupPath);
    void changeBackupState(bool state);
    void inserKey(QString disc, GroupKey key);
    void clearKeys();
    bool readKcdb(QString dbPath = "");
    bool writeKcdb(QString path = "");
    void setKeys(QMap<QString, GroupKey> keyList);
    QMap<QString, GroupKey> getKeys() const;
    Estring getKey() const;
    Estring getKey_in() const;
    Estring getSaveAESKeyPath() const;
    void setSaveAESKeyPath(Estring path);
    void setKey(Estring k);
    void setKey_in(Estring k);
    bool getBackupState() const;
    void setBackupState(bool isBackup);
    void setSavePath(QString path);
    void setBackupPath(QString path);
    QFile inFile;
    QFile outFile;
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
signals:
    void qml_msg_info(QString msg);
#endif

private:
    Kcdb_io ko;
    Estring key = Estring("259741592652");
    Estring key_in = Estring("259741592652");
    QString savePath = "";
    QString backupPath = "";
    QString saveAESPath = "";
    QString backupAESPath = "";
    QDataStream inStream;
    QDataStream outStream;
    // if backup, true; if save, false;
    bool backupState = false;

    void showAllKey();

    QMap<QString, GroupKey> groupKeyList;
    QMutableMapIterator<QString, GroupKey> gkptr = groupKeyList;
//    QMutableMapIterator<int, maintainKey>kcdb::mtkptr = maintainKey::mtKeyList;
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#else
    MessageBoxExX t;
#endif

};

#endif // KCDB_H
