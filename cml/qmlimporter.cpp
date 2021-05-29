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
        qml_msg_info("mkdir1: "+QString::number(workDir.mkpath(workPath)));
    }
    qml_msg_info("exists?: "+QString::number(workDir.exists(workPath)));
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
        // NOTE: 反正现在新密码在前端是加在最后一行，return个pos其实没啥用
        if(e.getVal() > disc){
            keyMap.insert(keyMap.count(), KeyMap(keyMap.count(), Estring(disc), Estring(account), Estring(password)));
            return pos;
        }
    }
    return pos;
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
