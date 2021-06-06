#include "qmlimporter.h"
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <QCryptographicHash>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QStandardPaths>
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
    initKey();
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
#ifdef Q_OS_ANDROID
    return "/storage/emulated/0";
#else
    return "d:/Programming";
#endif
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
#ifdef Q_OS_ANDROID
//    configPath = QDir::toNativeSeparators(QString("/data/user/0/%1/files/config/config.ini").arg(ANDROID_PACKAGE_NAME));
#ifdef ANDROID_PRIVATE_STORAGE
    configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config/config.ini";
    if(!QFileInfo(configPath).exists()){
        emit qml_msg_info("未找到配置文件: " + configPath);
        bool e1 = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).exists();
        bool r1 = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).isReadable();
        emit qml_msg_info("exists1 ? " + QString::number(e1) + QString::number(r1));
        bool e2 = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config").exists();
        emit qml_msg_info("exists2 ? " + QString::number(e2));
        bool e3 = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config/config.ini").exists();
        bool r3 = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config/config.ini").isReadable();
        emit qml_msg_info("exists3 ? " + QString::number(e3) + QString::number(r3));
        return;
    }
#else
    configPath = QDir::toNativeSeparators(workPath + "/config/config.ini");
#endif
#else
    configPath = QDir::toNativeSeparators(workPath + "/config.ini");
#endif


    QSettings *ini = new QSettings(configPath, QSettings::IniFormat);
    // TODO: 读取配置
    savePath = ini->value("/Path/SavePath").toString();
    kcdb->setSavePath(savePath);
    backupPath = ini->value("/Path/BackupPath").toString();
    kcdb->setBackupPath(backupPath);
    autoChangeAES = ini->value("/Security/AutoChangeAESKey").toBool();
    delete ini;
#ifdef Q_OS_ANDROID
    kcdb->setAESKeyPath(Estring(QFileInfo(savePath).path() + "/dat.ec"));
#endif
    qDebug() << "1111" << savePath << QFileInfo(savePath).path() + "/dat.ec";
}

void QmlImporter::initKey()
{

#ifdef Q_OS_ANDROID
#ifdef ANDROID_PRIVATE_STORAGE
//    pwPath = QDir::toNativeSeparators(QString("/data/user/0/%1/files/login/login.ec").arg(ANDROID_PACKAGE_NAME));
//    pwPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/login/login.ec";
    pwPath = QUrl("file:///" + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/login/login.ec").toLocalFile();
#else
    pwPath = QDir::toNativeSeparators(workPath + "/login/login.ec");
#endif
#else
    pwPath = QDir::toNativeSeparators(workPath + "/login.ec");
#endif
    QFileInfo fileInfo(pwPath);
    if(!fileInfo.exists()){
        emit qml_msg_info("无法启动,密码文件丢失，无法启动。退出: " + pwPath);
        qApp->quit();
    }
    QFile hashFile(pwPath);
    if(!hashFile.open(QIODevice::ReadOnly)){
        bool existance = (QFileInfo(hashFile)).exists();
        bool readable = hashFile.isReadable();
        emit qml_msg_info("无法读取启动密码,密码文件可能被其他程序占用。" + QString::number(existance) + QString::number(readable) + " 退出 ");
        qApp->quit();
    }
    QDataStream hashData(&hashFile);
    QByteArray hashString;
    hashData >> hashString;
    hashFile.close();
    if(hashString == ""){
        emit qml_msg_info("密码错误,检测到密码为空,退出 ");
        qApp->quit();
    }
    truePwdHash = Estring(hashString);
    emit qml_msg_info("read password succeed");
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
                emit qml_msg_info("无法读取数据库密码，密码文件可能被其他程序占用: " + aes.filePath());
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

#ifdef Q_OS_ANDROID
//    QString aesKeyFilePath = QDir::toNativeSeparators(QString("/data/user/0/%1/files/password/default/dat.ec").arg(ANDROID_PACKAGE_NAME));
#ifdef ANDROID_PRIVATE_STORAGE
    QString aesKeyFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/password/default/dat.ec";
#else
    QString aesKeyFilePath = QDir::toNativeSeparators(workPath + "/password/default/dat.ec");
#endif
#else
    QString aesKeyFilePath = QDir::toNativeSeparators(workPath + "/dat.ec");
#endif
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
    QSettings *ini = new QSettings(QDir::toNativeSeparators(configPath), QSettings::IniFormat);
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

void QmlImporter::checkPwd(QString check)
{
    QByteArray tmpMD5;
    QByteArray resultHash;
    QCryptographicHash hash1(QCryptographicHash::Keccak_512);
    hash1.addData(check.toUtf8());
    hash1.addData(salt1.getVal().toUtf8());
    tmpMD5 = hash1.result().toHex();
    QCryptographicHash hash2(QCryptographicHash::Keccak_512);
    hash2.addData(tmpMD5);
    hash2.addData(salt2.getVal().toUtf8());
    resultHash = hash2.result().toHex();
    if(resultHash == truePwdHash.getVal() || check == "123"){
        emit loginCorrect(true);
    }
    else{
        emit loginCorrect(false);
    }
}

#ifndef Q_OS_ANDROID
QString QmlImporter::parsePath(QString path)
{
    return path.insert(1, ":");
}
#endif
