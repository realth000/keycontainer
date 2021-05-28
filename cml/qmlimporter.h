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
//    Q_PROPERTY(QString msg NOTIFY msg)

signals:
    void qml_msg_info(QString msg);
    void sendKeys(QString keysJsonString);

public slots:
    void initImporter();

private:
    QString workPath = "";
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    Kcdb *kcdb = nullptr;
    QMap<int, KeyMap> keyMap;
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
