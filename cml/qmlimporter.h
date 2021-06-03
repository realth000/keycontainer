#ifndef QMLIMPORTER_H
#define QMLIMPORTER_H

#include <QObject>
#include "kcdb.h"
#include "keymapjsonengine.h"

class QmlImporter : public QObject
{
    Q_OBJECT
public:
    explicit QmlImporter(QObject *parent = nullptr);

signals:
    void qml_msg_info(QString msg);
    void qml_msg_update(QString msg);
    void sendKeys(QString keysJsonString);

public slots:
    void initImporter();
    int checkExistence(QString disc, QString account, QString password);
    void saveKeys();
    bool getAutoChangeAES() const;
    QString getWorkPath();
    QString getSavePath() const;
    QString getBackupPath() const;
    void setSavePath(QString path);
    void setAutoChangeAES(bool autoAES);
    void changeAESKey();
    void deleteKey(QVariant id);
    void syncKeyIndex();
#ifdef Q_OS_ANDROID
    void updateAndroidNotifier(QString msg);
#endif

private:
    bool autoChangeAES=false;
    QString workPath = "";
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    Kcdb *kcdb = nullptr;
    QMap<int, KeyMap> keyMap;
    QList<Estring> existsKeys;
    QString msg;
    bool autoBackupPath=false;
    int deletedKeysCount=0;


#ifdef Q_OS_ANDROID
    void initPermission();
#endif
    void initConfig();
    void initKeyData();
    bool checkDb(QString dbPath = "");
    void refreshAESKey();
    void syncKeyFromMap();
    void syncKeyMapToKcdb();
    void writeCheckFile(QString checkPath);
    void saveConfig();

#ifndef Q_OS_ANDROID
    QString parsePath(QString path);
#endif
};

#endif // QMLIMPORTER_H
