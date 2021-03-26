#include "kcdb.h"
#include "encryption/qaesencryption.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QIODevice>
#include <QDir>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

QString savePath = ".\\password.kcdb";
QString backupPath = ".\\password_backup.kcdb";

void AesClass::initTestCase(QString key_in)
{
    quint8 iv_16[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    for (int i=0; i<16; i++){iv.append(iv_16[i]);}
    if(key_in != ""){key = key_in;}
    hashKey = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
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

void Kcdb_io::output(QDataStream &outStream, QString data, AesClass *code)
{
    QByteArray outArray = code->CFB256Encrypt(data);
    quint8 len = outArray.length();
    char *a = new char(len);
    outStream.writeRawData(a,1);
    outStream.writeRawData(outArray, len);
}

void Kcdb_io::output(QDataStream &outStream, quint8 argc, AesClass *code)
{
    QByteArray outArray = code->CFB256Encrypt(QString::number(argc));
    quint8 len = outArray.length();
    char *a = new char(len);
    outStream.writeRawData(a,1);
    outStream.writeRawData(outArray, len);
}

void Kcdb_io::outKcdbHead(QDataStream &outStream, quint8 inType, AesClass *code)
{
    outStream.setVersion(QDataStream::Qt_5_12);
    output(outStream, inType, code);
}

void Kcdb_io::outKcdb(QDataStream &outStream, QString keyNameOrLevel, QString iKey, QString iCheckKey, quint8 EOF_, AesClass *code)
{
    outStream.setVersion(QDataStream::Qt_5_12);
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
    QString keyNameOrLevel, key, checkKey, iKey, iCheckKey;
    input(inStream, keyNameOrLevel, code);
    input(inStream, iKey, code);
    input(inStream, iCheckKey, code);
    key = iKey;
    checkKey = iCheckKey;
    QStringList readGroupKey ;
    readGroupKey << keyNameOrLevel<< key<< checkKey<<  iKey<<  iCheckKey;
    return readGroupKey;
}

QList<QList<QStringList> > Kcdb_io::inKcdb(QDataStream &inStream, AesClass *code)
{
    inStream.setVersion(QDataStream::Qt_5_12);
    quint8 head =  readKcdbHead(inStream, code);
    quint8 EOF_ = 0xff;
    if(head == BEGIN_OF_GROUPLEY){EOF_ =NEXT_IS_GROUPKEY;}
    else if(head == BEGIN_OF_MTKEY){EOF_ = NEXT_IS_MAINTAINKEY;}
    else{QMessageBox::information(NULL, QObject::tr("数据损坏"), QObject::tr("密码文件损坏。"));throw new EOF_Fail_Expection;}
    QList<QStringList> readGroupKeyList;
    QList<QStringList> readMaintainKeyList;
    while (EOF_ != END_OF_KCDB)
    {
        if(EOF_ == NEXT_IS_GROUPKEY){ readGroupKeyList << readKeys(inStream, code);}
        else if(EOF_ == NEXT_IS_MAINTAINKEY){ readMaintainKeyList << readKeys(inStream, code);}
        else{QMessageBox::information(NULL, QObject::tr("数据损坏"), QObject::tr("无法读取密码，请清除密码数据。"));throw new EOF_Fail_Expection;}
        input(inStream, EOF_, code);
    }
    QList<QList<QStringList>> readResult;
    readResult << readGroupKeyList << readMaintainKeyList;
    return readResult;
}

GroupKey::GroupKey(QString name, QString keyValue)
{
    this->account = name;
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

Kcdb::Kcdb(QString workPath)
{
    this->workPath = workPath;
}

void Kcdb::changeBackupState(bool state)
{
    this->backupState = state;
}

void Kcdb::inserKey(QString account, GroupKey key)
{
    this->groupKeyList.insert(account, key);
}

void Kcdb::readKcdb()
{
    QList<QList<QStringList>> readResult;
    if(!inFile.exists()){
        if(backupState){inFile.setFileName(QDir::toNativeSeparators(workPath + backupPath));}
        else{inFile.setFileName(QDir::toNativeSeparators(workPath + savePath));}
    }
    inFile.open(QIODevice::ReadOnly);
    inStream.setDevice(&inFile);
    Estring aesKeyFilePath = Estring(QDir::toNativeSeparators(QDir::toNativeSeparators(workPath + "/dat.ec")));
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
        }
        else{
            QMessageBox::information(NULL, QObject::tr("无法读取数据库密码"), QObject::tr("密码文件可能被其他程序占用。"));
            return;
        }
    }
    else{
        QMessageBox::information(NULL, QObject::tr("无法读取数据库密码"), QObject::tr("密码丢失。"));
        return;
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
            QString keyNameValue = BetweenKeysptr.next();
            QString keyValue = BetweenKeysptr.next();
            QString iKeyValue = BetweenKeysptr.next();
            QString checkKeyValue = BetweenKeysptr.next();
            QString iCheckKeyValue = BetweenKeysptr.next();
            if(keyValue == checkKeyValue && checkKeyValue == iCheckKeyValue && keyValue == checkKeyValue){
                GroupKey ng(keyNameValue, keyValue);
                inserKey(keyNameValue, ng);
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

}

void Kcdb::writeKcdb()
{
    gkptr.toFront();
//    mtkptr.toFront();
    if(backupState){outFile.setFileName(QDir::toNativeSeparators(workPath + backupPath));}
    else{outFile.setFileName(QDir::toNativeSeparators(workPath + savePath));}
    outFile.open(QIODevice::WriteOnly);
    outStream.setDevice(&outFile);
    AesClass *code = new AesClass;
    code->initTestCase(key_in.getVal());
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
        outKcdb(outStream, tempGroupKey.account,
                    tempGroupKey.getiKeyValue(), tempGroupKey.getiCheckKeyValue(), EOF_, code);
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

}
