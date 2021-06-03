﻿#include "qmlimporter.h"
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <QCryptographicHash>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif

QmlImporter::QmlImporter(QObject *parent) : QObject(parent)
{

}

void QmlImporter::initImporter()
{
#ifdef DEBUG_QML_SHOW_INFO
    qDebug() <<"start QmlImporter";
#endif
#ifdef Q_OS_ANDROID
    initPermission();
    QUrl uuu("file:///storage/emulated/0/KeyContainer");
    workPath = uuu.toLocalFile();
    QDir workDir(workPath);
    if(!workDir.exists()){
        emit qml_msg_info("mkdir1: "+QString::number(workDir.mkpath(workPath)));
    }
    emit qml_msg_info("exists?: "+QString::number(workDir.exists(workPath)));
#else
    workPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
#endif
    initConfig();
    initKeyData();
}

// TODO: 接收qml传来的账号密码用明文有点憨憨
// 不过现在整个qml都是裸奔的也就不差这点了
int QmlImporter::checkExistence(QString disc, QString account, QString password)
{
    if(existsKeys.count() == 0){
        return 1;
    }
    // pos<0证明已经存在，pos>0是该插入的位置（第1个之前，第2个之前……第n个之前，第n个之后）
    int pos = 0;
    foreach(Estring e, existsKeys){
        pos++;
        // NOTE: widget版中如果是已有密码的情况，是可以选择更新旧密码的
        if(e.getVal() == disc){
            return -pos;
        }
    }
    qDebug() << "before insert count:" << keyMap.count();
    keyMap.insert(keyMap.count(), KeyMap(keyMap.count(), Estring(disc), Estring(account), Estring(password)));
    qDebug() << "after insert count:" << keyMap.count();
    // NOTE: 反正现在新密码在前端是加在最后一行，return个pos其实没啥用
    return pos;
}

void QmlImporter::saveKeys()
{
    if(keyMap.count() < 1){
        return;
    }
    kcdb->setBackupState(false);
    QFileInfo saveInfo(savePath);
    QDir saveDir(saveInfo.path());
    if(saveDir.exists()){
        if(autoChangeAES){refreshAESKey();}
    }
    syncKeyMapToKcdb();
    emit qml_msg_info("正在保存数据...");
    if(kcdb->writeKcdb(savePath)){
        writeCheckFile(savePath);
        emit qml_msg_info("数据保存完成");
        bool autoBackupPathOld=autoBackupPath;
        autoBackupPath=true;
        // TODO: 保存的同时备份
//        on_backupKeyBtn_clicked();
        autoBackupPath=autoBackupPathOld;
    }
    else{
        emit qml_msg_info("保存失败");
    }
}

bool QmlImporter::getAutoChangeAES() const
{
    return autoChangeAES;
}

QString QmlImporter::getWorkPath()
{
    QString s= workPath.replace("\\", "/");
    workPath = QDir::toNativeSeparators(workPath);
    return s;
}

QString QmlImporter::getSavePath() const
{
    return savePath;
}

QString QmlImporter::getBackupPath() const
{
    return backupPath;
}

void QmlImporter::setSavePath(QString path)
{
//    path.replace("file:///", "");
#ifndef Q_OS_ANDROID
    savePath = parsePath(path);
#else
    savePath = path;
#endif
    kcdb->setSavePath(savePath + "/pw.kcdb");
    saveConfig();
}

void QmlImporter::setAutoChangeAES(bool autoAES)
{
    autoChangeAES = autoAES;
    saveConfig();
}

#ifdef Q_OS_ANDROID
void QmlImporter::updateAndroidNotifier(QString msg)
{
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(msg);
    QAndroidJniObject::callStaticMethod<void>("org/th000/keycontainer/application/SelfApplication",
                                       "notify",
                                       "(Ljava/lang/String;)V",
                                       javaNotification.object<jstring>());
}
#endif

#ifdef Q_OS_ANDROID
void QmlImporter::initPermission()
{
    QtAndroid::PermissionResult r = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");
    if(r == QtAndroid::PermissionResult::Denied) {
       QtAndroid::requestPermissionsSync(QStringList()<<"android.permission.READ_EXTERNAL_STORAGE");
    }
    r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync(QStringList()<<"android.permission.WRITE_EXTERNAL_STORAGE");
    }
    r = QtAndroid::checkPermission("android.permission.MANAGE_EXTERNAL_STORAGE");
    if(r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync(QStringList()<<"android.permission.MANAGE_EXTERNAL_STORAGE");
    }
}
#endif

void QmlImporter::initConfig()
{
#ifdef DEBUG_QML_SHOW_INFO
    emit qml_msg_info("test msg in QmlImporter.initConfig");
#endif
    savePath = QDir::toNativeSeparators(workPath + savePath);
    backupPath = QDir::toNativeSeparators(workPath + backupPath);
    kcdb = new Kcdb(workPath);
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
    connect(kcdb, &Kcdb::qml_msg_info, this, &QmlImporter::qml_msg_info);
#endif
    if(!QFileInfo(QDir::toNativeSeparators(workPath + "/config.ini")).exists()){
        emit qml_msg_info("未找到配置文件");
        return;
    }
    QSettings *ini = new QSettings(QDir::toNativeSeparators(workPath + "/config.ini"), QSettings::IniFormat);
    // TODO: 读取配置
    savePath = ini->value("/Path/SavePath").toString();
    kcdb->setSavePath(savePath);
    backupPath = ini->value("/Path/BackupPath").toString();
    kcdb->setBackupPath(backupPath);
    autoChangeAES = ini->value("/Security/AutoChangeAESKey").toBool();
    delete ini;
}

void QmlImporter::initKeyData()
{
    if(!(QFileInfo(savePath)).exists()){
        emit qml_msg_info("未找到数据文件: " + savePath);
        return;
    }

    if(!checkDb()){
        emit qml_msg_info("数据校验未通过，拒绝读取数据");
        return;
    }
    if(kcdb->readKcdb()){
        syncKeyFromMap();
    }
    else{
        emit qml_msg_info("kcdb->readKcdb() failed");
    }
}

bool QmlImporter::checkDb(QString dbPath)
{
    QFileInfo configFileInfo;
    QFile dbFile;
    QString hashFilePath;
    if(dbPath.isEmpty()){
        configFileInfo.setFile(savePath+ ".chf");
        dbFile.setFileName(savePath);
        hashFilePath = savePath + ".chf";
    }
    else{
        configFileInfo.setFile(dbPath+ ".chf");
        dbFile.setFileName(dbPath);
        hashFilePath = dbPath + ".chf";
    }
    if(configFileInfo.exists()){
        QByteArray tmpMD5;
        QByteArray resultHash;
        dbFile.open(QIODevice::ReadOnly);
        QCryptographicHash hash1(QCryptographicHash::Keccak_512);
        hash1.addData(&dbFile);
        QString salt1 = "撒差盐可空拟中";
        hash1.addData(salt1.toUtf8());
        tmpMD5 = hash1.result().toHex();
        dbFile.close();
        QCryptographicHash hash2(QCryptographicHash::Keccak_512);
        hash2.addData(tmpMD5);
        QString salt2 = "未因若风柳起絮";
        hash2.addData(salt2.toUtf8());
        resultHash = hash2.result().toHex();

        QFile hashFile(hashFilePath);
        if(hashFile.open(QIODevice::ReadOnly)){
            QDataStream hashData(&hashFile);
            hashData.setVersion(QDataStream::Qt_5_12);
            QByteArray hashString;
            hashData >> hashString;
            hashFile.close();

            // Update(2.1.5): Encryption on *.chf
            Estring key_in;
            QFileInfo aes(kcdb->getAESKeyPath().getVal());
            if(aes.exists()){
                QFile aesHashFile(kcdb->getAESKeyPath().getVal());
                if(aesHashFile.open(QIODevice::ReadOnly)){
                    QDataStream aesStream(&aesHashFile);
                    QByteArray aesString;
                    aesStream >> aesString;
                    aesHashFile.close();
                    AesClass *de = new AesClass;
                    de->initTestCase(kcdb->getKey().getVal());
                    key_in = Estring(de->CFB256Decrypt(aesString).toUtf8());
                    delete de;
                }
            }
            else{
                emit qml_msg_info("无法读取数据库密码，密码文件可能被其他程序占用。");
                return false;
            }
            AesClass *ec = new AesClass;
            ec->initTestCase(key_in.getVal());
            QByteArray hashString_de = ec->CFB256Decrypt(hashString).toUtf8();
            delete ec;

            if(hashString_de.compare(resultHash) != 0){
                emit qml_msg_info("数据库被篡改，校验得数据库已被篡改，建议读取备份。");
                return false;
            }
            return true;
        }
        else{
            emit qml_msg_info("无法校验数据库，数据库校验文件无法打开。");
            return false;
        }
    }
    else{
        emit qml_msg_info("数据库可能已被篡改，检测不到数据库的校验文件。");
        return false;
    }
}

void QmlImporter::refreshAESKey()
{
    int max = 256;
    QString tmp = QString("12TocJn%BFde6Ng}0fGSY5s34H-PIwWEhi+#x)DuvptklabZUKq8z9jQmM$VA{R7C[X(rLOy");
    QString str;
    QTime t;
    t = QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    for(int i=0;i<max;i++)
    {
        int len = qrand()%tmp.length();
        str[i] =tmp.at(len);
    }
    kcdb->setKey_in(str);
    QString aesKeyFilePath = QDir::toNativeSeparators(workPath + "/dat.ec");
    QFile aesFile(aesKeyFilePath);
    if(aesFile.open(QIODevice::ReadWrite)){
        QDataStream aesStream(&aesFile);
        aesStream.setVersion(QDataStream::Qt_5_12);
        AesClass *de = new AesClass;
        de->initTestCase(kcdb->getKey().getVal());
        QByteArray resultAes = de->CFB256Encrypt(kcdb->getKey_in().getVal());
        aesStream << resultAes;
#ifdef DEBUG_SHOW_KEYS
        qDebug() << "refreshAESKey: write new AES key(Encrypted)=" << resultAes;
#endif
        aesFile.close();
        emit qml_msg_info("已刷新AES密钥");
    }
    else{emit qml_msg_info("无法保存密码,密码文件被其他程序占用，请重试。");}
}

void QmlImporter::syncKeyFromMap()
{
    QMap<QString, GroupKey> tmp = kcdb->getKeys();
    keyMap.clear();
    QMap<QString, GroupKey>::const_iterator t = tmp.begin();
    int count=0;
    while (t != tmp.cend()) {
         KeyMap nKey(count, t.value().disc, t.value().account, t.value().key);
        keyMap.insert(count, nKey);
        existsKeys.append(t.value().disc);
        count++;
        t++;
    }
    emit sendKeys(KeyMapJsonEngine::keyMapToJson(keyMap));
    qDebug() << "read .kcdb successed:" << count << "keys";
}

void QmlImporter::syncKeyMapToKcdb()
{
    QMap<QString, GroupKey> tmp;
    kcdb->clearKeys();
    QMap<int, KeyMap>::const_iterator t = keyMap.begin();
    while (t != keyMap.cend()) {
        GroupKey nKey(t.value().disc, t.value().account, t.value().password);

        tmp.insert(t.value().disc.getVal(), nKey);
        t++;
    }
    kcdb->setKeys(tmp);
}

void QmlImporter::writeCheckFile(QString checkPath)
{
    QFileInfo configFileInfo(checkPath);
    if(configFileInfo.exists()){
        QByteArray tmpMD5;
        QByteArray resultHash;
        QFile dbFile(checkPath);
        dbFile.open(QIODevice::ReadOnly);
        QCryptographicHash hash1(QCryptographicHash::Keccak_512);
        hash1.addData(&dbFile);
        QString salt1 = "撒差盐可空拟中";
        hash1.addData(salt1.toUtf8());
        tmpMD5 = hash1.result().toHex();
        dbFile.close();
        QCryptographicHash hash2(QCryptographicHash::Keccak_512);
        hash2.addData(tmpMD5);
        QString salt2 = "未因若风柳起絮";
        hash2.addData(salt2.toUtf8());
        resultHash = hash2.result().toHex();

        // Update(2.1.5): Encryption on *.chf
        AesClass *ec = new AesClass;
        ec->initTestCase(kcdb->getKey_in().getVal());
        QByteArray resultHash_en = ec->CFB256Encrypt(resultHash);
        delete ec;

        QString hashFilePath = checkPath + ".chf";
        QFile hashFile(hashFilePath);
        hashFile.open(QIODevice::WriteOnly);
        QDataStream outData(&hashFile);
        outData.setVersion(QDataStream::Qt_5_12);
        outData << resultHash_en;
        hashFile.close();
        emit qml_msg_info("已生成校验文件");
    }
    else{emit qml_msg_info("无法生成校验文件,无法生成校验文件，建议重新保存");}
}

void QmlImporter::saveConfig()
{
    QSettings *ini = new QSettings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/config.ini"), QSettings::IniFormat);
    ini->setValue("/Path/SavePath", savePath.replace("\\", "/"));
    ini->setValue("/Path/BackupPath", backupPath.replace("\\", "/"));
    // 同时还要同步两个变量
//    kcdb->setBackupPath(ui->backupPathLE->text());
    ini->setValue("/Security/AutoChangeAESKey", autoChangeAES);
    ini->setValue("/Common/AutoBackupPath", autoBackupPath);
    delete ini;
}

void QmlImporter::changeAESKey()
{
    if(!autoChangeAES){refreshAESKey();};
    saveKeys();
    emit qml_msg_update("SETTINGS_AESKEY_UPDATE_FINISH");
}

void QmlImporter::deleteKey(QVariant id)
{
    keyMap.remove(id.toInt());
    existsKeys.removeAt(id.toInt());
    deletedKeysCount++;
    qDebug() << "deleting:" << id;
}

void QmlImporter::syncKeyIndex()
{
    int pos=0;
    QMap<int, KeyMap>::const_iterator t = keyMap.begin();
    while((deletedKeysCount > 0) && (t != keyMap.end())){
        if(t.key() != pos){
            KeyMap newItem (t.value().id, t.value().disc, t.value().account, t.value().password);
            keyMap.insert(pos, newItem);
            keyMap.remove(t.key());
        }
        pos++;
        t++;
        deletedKeysCount--;
    }
}

#ifndef Q_OS_ANDROID
QString QmlImporter::parsePath(QString path)
{
    return path.insert(1, ":");
}
#endif
