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
    void sendKeys(QString keysJsonString);

public slots:
    void initImporter();
    int checkExistence(QString disc, QString account, QString password);
#ifdef Q_OS_ANDROID
    void updateAndroidNotifier(QString msg);
#endif

private:
    QString workPath = "";
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    Kcdb *kcdb = nullptr;
    QMap<int, KeyMap> keyMap;
    QList<Estring> existsKeys;
    QString msg;

#ifdef Q_OS_ANDROID
    void initPermission();
#endif
    void initConfig();
    void initKeyData();
    bool checkDb(QString dbPath = "");
    void syncKeyFromMap();
};

#endif // QMLIMPORTER_H
