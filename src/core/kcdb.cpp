#include "kcdb.h"
#include "encryption/qaesencryption.h"
#include <QCryptographicHash>
#include <QIODevice>
#include <QDir>
#include <QDebug>
#include "debugshowoptions.h"
#include "commoninclude.h"
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#else
#include "qssinstaller.h"
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

void Kcdb_io::output(QDataStream &outStream, Estring data, AesClass *AESMachine)
{
    QByteArray outArray = AESMachine->CFB256Encrypt(data.getVal());
    outStream << outArray;
}

void Kcdb_io::output(QDataStream &outStream, int argc, AesClass *AESMachine)
{
    QByteArray outArray = AESMachine->CFB256Encrypt(QString::number(argc));
    outStream << outArray;
}

void Kcdb_io::outKcdbHead(QDataStream &outStream, int data, AesClass *AESMachine)
{
    output(outStream, data, AESMachine);
}

void Kcdb_io::outKcdb(QDataStream &outStream, Estring disc, Estring account, Estring iKey, Estring iCheckKey, int kcdb_pos, AesClass *AESMachine)
{
    output(outStream, disc, AESMachine);
    output(outStream, account, AESMachine);
    output(outStream, iKey, AESMachine);
    output(outStream, iCheckKey, AESMachine);
    output(outStream, kcdb_pos, AESMachine);
}

void Kcdb_io::input(QDataStream &inStream, QString &data, AesClass *AESMachine)
{

    QByteArray inArray;
    inStream >> inArray;
    data = AESMachine->CFB256Decrypt(inArray);
}

void Kcdb_io::input(QDataStream &inStream, int &argc, AesClass *AESMachine)
{

    QByteArray inArray;
    inStream >> inArray;
    argc= (AESMachine->CFB256Decrypt(inArray)).toInt();
}

int Kcdb_io::readKcdbHead(QDataStream &inStream, AesClass *AESMachine)
{
    int inType;
    input(inStream, inType, AESMachine);
    return inType;
}

QStringList Kcdb_io::readKeys(QDataStream &inStream, AesClass *AESMachine)
{
    QString disc, account, key, checkKey, iKey, iCheckKey;
    input(inStream, disc, AESMachine);
    input(inStream, account, AESMachine);
    input(inStream, iKey, AESMachine);
    input(inStream, iCheckKey, AESMachine);
    key = iKey;
    checkKey = iCheckKey;
    QStringList readGroupKey ;
    readGroupKey << disc << account<< key<< checkKey<<  iKey<<  iCheckKey;
    return readGroupKey;
}

QList<QList<QStringList> > Kcdb_io::inKcdb(QDataStream &inStream, AesClass *AESMachine)
{
    int head =  readKcdbHead(inStream, AESMachine);
    int kcdb_pos = KCDB_FILE_END;
    if(head == KCDB_ENGINE_VERSION){
        kcdb_pos =KCDB_WRITE_KEY_CONTINUE;
    }
    else{
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
        emit qml_msg_info("数据版本错误,密码数据记录格式版本不正确，无法读取");
#else
        MessageBoxExY::information("数据版本错误", "密码数据记录格式版本不正确，无法读取");
#endif
    }
    QList<QStringList> readGroupKeyList;
    while (kcdb_pos != KCDB_FILE_END){
        if(kcdb_pos == KCDB_WRITE_KEY_CONTINUE){
            readGroupKeyList << readKeys(inStream, AESMachine);
        }
        else{
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
        emit qml_msg_info("数据损坏,无法读取密码，请清除密码数据。");
#else
            MessageBoxExY::information("数据损坏",QString("无法读取密码，请清除密码数据。\n"
                                              "readhead=0x%1\n"
                                              "datapose=0x%2").arg(head, 8, 16, QLatin1Char('0')).arg(kcdb_pos, 8, 16, QLatin1Char('0')));
#endif
        throw new EOF_Fail_Expection;
        }
        input(inStream, kcdb_pos, AESMachine);
    }
    QList<QList<QStringList>> readResult;
    readResult << readGroupKeyList;
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

Kcdb::Kcdb(QString savePath, QString backupPath)
{
    this->savePath = QDir::toNativeSeparators(savePath);
    this->backupPath = QDir::toNativeSeparators(backupPath);
    this->saveAESPath = QDir::toNativeSeparators(QFileInfo(savePath).path() + KEYDB_PASSWD_FILE_NAME);
    this->backupAESPath = QDir::toNativeSeparators(QFileInfo(backupPath).path() + KEYDB_PASSWD_FILE_NAME);
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
    connect(&ko, &Kcdb_io::qml_msg_info, this, &Kcdb::qml_msg_info);
#endif
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

bool Kcdb::readKcdb(QString dbPath)
{
    QString aesPath;
    if(dbPath.isEmpty()){
        inFile.setFileName(savePath);
        aesPath = saveAESPath;
        if(!inFile.exists()){
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
            emit qml_msg_info("数据库不存在,切换读取备份数据: " + inFile.fileName());
#else
            MessageBoxExY::information("数据库不存在","切换读取备份数据");
#endif
            inFile.setFileName(backupPath);
            aesPath = backupAESPath;
            if(!inFile.exists()){
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
                emit qml_msg_info("找不到数据库,找不到数据库及数据库备份，无法读取密码。");
#else
                MessageBoxExY::information("找不到数据库","找不到数据库及数据库备份，无法读取密码。");
#endif
                return false;
            }
        }

    }
    else{
        inFile.setFileName(dbPath);
        if(!inFile.exists()){
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
            emit qml_msg_info("找不到数据库,找不到数据库及数据库备份，无法读取密码。");
#else
            MessageBoxExY::information("找不到数据库","找不到数据库及数据库备份，无法读取密码。");
#endif
            return false;
        }
        aesPath = QDir::toNativeSeparators(QFileInfo(inFile).path().replace("\\", "/") + KEYDB_PASSWD_FILE_NAME);
    }
    QList<QList<QStringList>> readResult;
    if(!inFile.open(QIODevice::ReadOnly)){
        return false;
    }
    inStream.setDevice(&inFile);
    inStream.setVersion(QDataStream::Qt_5_11);
    Estring aesKeyFilePath = Estring(QDir::toNativeSeparators(aesPath));
    QFileInfo aes(aesKeyFilePath.getVal());
#ifdef DEBUG_SHOW_KEYS
    qDebug() << "readKcdb: getAESKey in" << aesKeyFilePath.getVal();
#endif

#ifdef DEBUG_SHOW_IO_PATH
    qDebug() << "readKcdb:" << "read data from " << inFile.fileName();
    qDebug() << "readKcdb:" << "write AES key data from " << aes.filePath();
#endif
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
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
            emit qml_msg_info("无法读取数据库密码,密码文件可能被其他程序占用。");
#else
            MessageBoxExY::information("无法读取数据库密码", "密码文件可能被其他程序占用。");
#endif
            return false;
        }
    }
    else{
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
            emit qml_msg_info("无法读取数据库密码,密码丢失。");
#else
            MessageBoxExY::information("无法读取数据库密码", "密码丢失。");
#endif
        return false;
    }
    AesClass *AESMachine = new AesClass;
    AESMachine->initTestCase(key_in.getVal());
    readResult = inKcdb(inStream, AESMachine);
    groupKeyList.clear();
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
    }
    inFile.close();
    delete AESMachine;
    return true;
}

bool Kcdb::writeKcdb(QString inputPath)
{
    gkptr.toFront();
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
    outStream.setVersion(QDataStream::Qt_5_11);
    AesClass *AESMachine = new AesClass;
    AESMachine->initTestCase(key_in.getVal());
#ifdef DEBUG_SHOW_KEYS
    qDebug() << "writeKcdb: write .kcdb file" << outFile.fileName() << "with key_in:" << key_in.getVal();
#endif
#ifdef DEBUG_SHOW_IO_PATH
    qDebug() << "writeKcdb:" << "write data to " << outFile.fileName();
#endif
    // 写入Kcdb头部信息
    if(gkptr.hasNext()){
        int kcdbVersion = KCDB_ENGINE_VERSION;
        outKcdbHead(outStream, kcdbVersion, AESMachine);
    }
    // 写入Keys数据
    int kcdb_pos;
    while(gkptr.hasNext()){
        GroupKey keyToWrite = gkptr.next().value();
        gkptr.hasNext() ? kcdb_pos = KCDB_WRITE_KEY_CONTINUE : kcdb_pos = KCDB_FILE_END;
        outKcdb(outStream, keyToWrite.disc, keyToWrite.account, keyToWrite.getiKey(), keyToWrite.getiCheckKey(), kcdb_pos, AESMachine);
    }
    outFile.close();
    delete AESMachine;
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

Estring Kcdb::getSaveAESKeyPath() const
{
    return Estring(this->saveAESPath);
}

void Kcdb::setSaveAESKeyPath(Estring path)
{
    this->saveAESPath = path.getVal();
}

// TODO: 存储密码？
void Kcdb::setKey(Estring k)
{
    this->key = k;
}

void Kcdb::setKey_in(Estring k)
{
    this->key_in = k;
}

bool Kcdb::getBackupState() const
{
    return this->backupState;
}

void Kcdb::setBackupState(bool isBackup)
{
    this->backupState = isBackup;
}

void Kcdb::setSavePath(QString path)
{
    this->savePath = path;
    this->saveAESPath =QDir::toNativeSeparators((QFileInfo(path)).path().replace("\\", "/") + KEYDB_PASSWD_FILE_NAME);
}

void Kcdb::setBackupPath(QString path)
{
    this->backupPath = path;
    this->backupAESPath =QDir::toNativeSeparators((QFileInfo(path)).path().replace("\\", "/") + KEYDB_PASSWD_FILE_NAME);
}
