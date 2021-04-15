#include "kcdb.h"
#include "encryption/qaesencryption.h"
#include <QCryptographicHash>
#include <QIODevice>
#include <QDir>
#include <QDebug>
#include "debugshowoptions.h"
#include "qssinstaller.h"


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

void AesClass::initTestCase(QString key_in)
{
    quint8 iv_16[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    for (int i=0; i<16; i++){iv.append(iv_16[i]);}
    if(key_in != ""){aesKey = key_in;}
    hashKey = QCryptographicHash::hash(aesKey.toUtf8(), QCryptographicHash::Sha256);

#ifdef DEBUG_SHOW_KEYS
    qDebug() << "AesClass::initTestCase: set AesKey =" << aesKey << ", hash key =" <<hashKey;
#endif
}

QByteArray AesClass::CFB256Encrypt(QString inputStr)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CFB, QAESEncryption::PKCS7);
    QByteArray result = encryption.encode(inputStr.toUtf8(), hashKey, iv);
    return result;
}

QString AesClass::CFB256Decrypt(QByteArray inputStr)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CFB, QAESEncryption::PKCS7);
    QByteArray decode = encryption.removePadding(encryption.decode(inputStr, hashKey, iv));
    QString  result = QString::fromUtf8(decode);
    return  result;
}


Kcdb_io::Kcdb_io()
{

}

void Kcdb_io::output(QDataStream &outStream, Estring data, AesClass *code)
{
    QByteArray outArray = code->CFB256Encrypt(data.getVal());
    quint8 len = outArray.length();
    char *a = new char(len);
    outStream.writeRawData(a,1);
    outStream.writeRawData(outArray, len);
    delete a;
}

void Kcdb_io::output(QDataStream &outStream, quint8 argc, AesClass *code)
{
    QByteArray outArray = code->CFB256Encrypt(QString::number(argc));
    quint8 len = outArray.length();
    char *a = new char(len);
    outStream.writeRawData(a,1);
    outStream.writeRawData(outArray, len);
    delete a;
}

void Kcdb_io::outKcdbHead(QDataStream &outStream, quint8 inType, AesClass *code)
{
    outStream.setVersion(QDataStream::Qt_5_12);
    output(outStream, inType, code);
}

void Kcdb_io::outKcdb(QDataStream &outStream, Estring disc, Estring keyNameOrLevel, Estring iKey, Estring iCheckKey, quint8 EOF_, AesClass *code)
{
    outStream.setVersion(QDataStream::Qt_5_12);
    output(outStream, disc, code);
    output(outStream, keyNameOrLevel, code);
    output(outStream, iKey, code);
    output(outStream, iCheckKey, code);
    output(outStream, EOF_, code);
}

void Kcdb_io::input(QDataStream &inStream, QString &data, AesClass *code)
{
    QByteArray inArray;
    quint8 len = 0;
    char *t = new char[1];
    inStream.readRawData(t, 1);
    len = t[0];
    int c = 0;
    while(c<len){
        t[0] = '\x00';
        inStream.readRawData(t, 1);
        inArray.append(t[0]);
        c++;
    }
    data = code->CFB256Decrypt(inArray);
    delete [] t;
}

void Kcdb_io::input(QDataStream &inStream, quint8 &argc, AesClass *code)
{
    QByteArray inArray;
    quint8 len = 0;
    char *t = new char[1];
    inStream.readRawData(t, 1);
    len = t[0];
    int c = 0;
    while(c<len){
        t[0] = '\x00';
        inStream.readRawData(t, 1);
        inArray.append(t[0]);
        c++;
    }
    argc= (quint8)(code->CFB256Decrypt(inArray)).toInt(nullptr,10);
    delete [] t;
}

quint8 Kcdb_io::readKcdbHead(QDataStream &inStream, AesClass *code)
{
    quint8 inType;
    input(inStream, inType, code);
    return inType;
}

QStringList Kcdb_io::readKeys(QDataStream &inStream, AesClass *code)
{
    QString disc, keyNameOrLevel, key, checkKey, iKey, iCheckKey;
    input(inStream, disc, code);
    input(inStream, keyNameOrLevel, code);
    input(inStream, iKey, code);
    input(inStream, iCheckKey, code);
    key = iKey;
    checkKey = iCheckKey;
    QStringList readGroupKey ;
    readGroupKey << disc << keyNameOrLevel<< key<< checkKey<<  iKey<<  iCheckKey;
    return readGroupKey;
}

QList<QList<QStringList> > Kcdb_io::inKcdb(QDataStream &inStream, AesClass *code)
{
    inStream.setVersion(QDataStream::Qt_5_12);
    quint8 head =  readKcdbHead(inStream, code);
    quint8 EOF_ = 0xff;
    if(head == BEGIN_OF_GROUPLEY){EOF_ =NEXT_IS_GROUPKEY;}
    else if(head == BEGIN_OF_MTKEY){EOF_ = NEXT_IS_MAINTAINKEY;}
    else{t.information("数据损坏", "密码文件损坏。");throw new EOF_Fail_Expection;}
    QList<QStringList> readGroupKeyList;
    QList<QStringList> readMaintainKeyList;
    while (EOF_ != END_OF_KCDB)
    {
        if(EOF_ == NEXT_IS_GROUPKEY){ readGroupKeyList << readKeys(inStream, code);}
        else if(EOF_ == NEXT_IS_MAINTAINKEY){ readMaintainKeyList << readKeys(inStream, code);}
        else{t.information("数据损坏", "无法读取密码，请清除密码数据。");throw new EOF_Fail_Expection;}
        input(inStream, EOF_, code);
    }
    QList<QList<QStringList>> readResult;
    readResult << readGroupKeyList << readMaintainKeyList;
    return readResult;
}

GroupKey::GroupKey(Estring disc, Estring account, Estring keyValue)
{
    this->disc = disc;
    this->account = account;
    this->key = keyValue;
    this->checkKey = keyValue;
    this->iKey = keyValue;
    this->iCheckKey = keyValue;
}

GroupKey::GroupKey(QString disc, QString account, QString keyValue)
{
        this->disc.setVal(disc);
        this->account.setVal(account);
        this->key.setVal(keyValue);
        this->checkKey.setVal(keyValue);
        this->iKey.setVal(keyValue);
        this->iCheckKey.setVal(keyValue);

}

QString GroupKey::getiKeyValue() const
{
    return iKey.getVal();
}

QString GroupKey::getiCheckKeyValue() const
{
    return iCheckKey.getVal();
}

Estring GroupKey::getiKey() const
{
    return iKey;
}

Estring GroupKey::getiCheckKey() const
{
    return iCheckKey;
}

Kcdb::Kcdb(QString workPath)
{
    this->workPath =  QDir::toNativeSeparators(workPath);
    this->savePath = QDir::toNativeSeparators(this->workPath + savePath);
    this->backupPath = QDir::toNativeSeparators(this->workPath + backupPath);
    this->aesPath = QDir::toNativeSeparators(this->workPath + aesPath);
}

void Kcdb::changeBackupState(bool state)
{
    this->backupState = state;
}

void Kcdb::inserKey(QString disc, GroupKey key)
{
    this->groupKeyList.insert(disc, key);
}

void Kcdb::clearKeys()
{
    this->gkptr.toFront();
    groupKeyList.clear();
}

bool Kcdb::readKcdb()
{
    QList<QList<QStringList>> readResult;
    inFile.setFileName(savePath);
    if(!inFile.exists()){
        t.information("数据库不存在", "切换读取备份数据。");
        inFile.setFileName(backupPath);
        if(!inFile.exists()){
            t.information("找不到数据库", "找不到数据库及数据库备份，无法读取密码。");
            return false;
        }
    }
    inFile.open(QIODevice::ReadOnly);
    inStream.setDevice(&inFile);
    Estring aesKeyFilePath = Estring(QDir::toNativeSeparators(aesPath));
#ifdef DEBUG_SHOW_KEYS
    qDebug() << "readKcdb: getAESKey in" << aesKeyFilePath.getVal();
#endif
    QFileInfo aes(aesKeyFilePath.getVal());
    if(aes.exists()){
        QFile aesHashFile(aesKeyFilePath.getVal());
        if(aesHashFile.open(QIODevice::ReadOnly)){
            QDataStream aesStream(&aesHashFile);
            QByteArray aesString;
            aesStream >> aesString;
            aesHashFile.close();
            AesClass *de = new AesClass;
            de->initTestCase(key.getVal());
            key_in = Estring(de->CFB256Decrypt(aesString));
            delete de;
        }
        else{
            t.information("无法读取数据库密码", "密码文件可能被其他程序占用。");
            return false;
        }
    }
    else{
        t.information("无法读取数据库密码", "密码丢失。");
        return false;
    }
    AesClass *code = new AesClass;
    code->initTestCase(key_in.getVal());
    readResult = inKcdb(inStream, code);
    groupKeyList.clear();
//    maintainKey::mtKeyList.clear();
    QListIterator<QList<QStringList>> Noptr = readResult;
    if(Noptr.hasNext()){
        QList<QStringList> classesOfKeys = Noptr.next();
        for(QListIterator<QStringList> classListptr = classesOfKeys; classListptr.hasNext(); classListptr.next()){
            QStringList readClassKeyList = classListptr.peekNext();

            // TOOD: unsafe in reading kcdb.
            QListIterator<QString>BetweenKeysptr = readClassKeyList;
            QString disc = BetweenKeysptr.next();
            QString keyNameValue = BetweenKeysptr.next();
            QString keyValue = BetweenKeysptr.next();
            QString iKeyValue = BetweenKeysptr.next();
            QString checkKeyValue = BetweenKeysptr.next();
            QString iCheckKeyValue = BetweenKeysptr.next();
            if(keyValue == checkKeyValue && checkKeyValue == iCheckKeyValue && keyValue == checkKeyValue){
                GroupKey ng(disc, keyNameValue, keyValue);
                inserKey(disc, ng);
            }
        }
//        classesOfKeys = Noptr.next();
//        for(QListIterator<QStringList> classListptr = classesOfKeys; classListptr.hasNext(); classListptr.next()){
//            QStringList readClassKeyList = classListptr.peekNext();
//            QListIterator<QString>BetweenKeysptr = readClassKeyList;
//            QString keyLevel = BetweenKeysptr.next();
//            QString keyValue = BetweenKeysptr.next();
//            QString iKeyValue = BetweenKeysptr.next();
//            QString checkKeyValue = BetweenKeysptr.next();
//            QString iCheckKeyValue = BetweenKeysptr.next();
//            maintainKey(keyLevel, keyValue, iKeyValue, checkKeyValue, iCheckKeyValue);
//        }
    }
    inFile.close();
//    qDebug() << "readed kcdb items:" << groupKeyList.size();
    delete code;
    return true;
}

bool Kcdb::writeKcdb(QString inputPath)
{
    gkptr.toFront();
//    mtkptr.toFront();
    if(inputPath == ""){
        if(backupState){outFile.setFileName(backupPath);}
        else{outFile.setFileName(savePath);}

    }
    else{
        outFile.setFileName(inputPath);
    }
    if(!outFile.open(QIODevice::ReadWrite)){
        return false;
    }
    outStream.setDevice(&outFile);
    AesClass *code = new AesClass;
    code->initTestCase(key_in.getVal());
#ifdef DEBUG_SHOW_KEYS
    qDebug() << "writeKcdb: write .kcdb file" << outFile.fileName() << "with key_in:" << key_in.getVal();
#endif
    if(gkptr.hasNext()){
        quint8 inType = BEGIN_OF_GROUPLEY;
        outKcdbHead(outStream, inType, code);
    }
    quint8 EOF_;
    while(gkptr.hasNext())
    {
        GroupKey tempGroupKey = gkptr.next().value();

        if(gkptr.hasNext()){EOF_ = NEXT_IS_GROUPKEY;}
        else {
            if(/*mtkptr.hasNext()*/false){ EOF_ = NEXT_IS_MAINTAINKEY;}
            else{ EOF_ = END_OF_KCDB;}
        }
        outKcdb(outStream, tempGroupKey.disc, tempGroupKey.account,
                    tempGroupKey.getiKey(), tempGroupKey.getiCheckKey(), EOF_, code);
    }
//    while(mtkptr.hasNext())
//    {
//        maintainKey tempMaintainKey = mtkptr.next().value();
//        if(mtkptr.hasNext()){ EOF_ = NEXT_IS_MAINTAINKEY;}
//        else{ EOF_ = END_OF_KCDB;}
//        outKcdb(outStream,QString::number(tempMaintainKey.index),
//                tempMaintainKey.iKey.operator QString(), tempMaintainKey.iCheckKey.operator QString(), EOF_, code);
//    }
    outFile.close();
    delete code;
    return true;
}

void Kcdb::setKeys(QMap<QString, GroupKey> keyList)
{
    this->groupKeyList = keyList;
}

QMap<QString, GroupKey> Kcdb::getKeys() const
{
    return this->groupKeyList;
}

Estring Kcdb::getKey() const
{
    return this->key;
}

Estring Kcdb::getKey_in() const
{
    return this->key_in;
}

Estring Kcdb::getAESKeyPath() const
{
    return Estring(this->aesPath);
}

void Kcdb::setKey(QString k)
{
    this->key.setVal(k);
}

void Kcdb::setKey_in(QString k)
{
    this->key_in.setVal(k);
}

void Kcdb::setBackupState(bool isBackup)
{
    this->backupState = isBackup;
}

void Kcdb::setSavePath(QString path)
{
    this->savePath = path;
}

void Kcdb::setBackupPath(QString path)
{
    this->backupPath = path;
}
