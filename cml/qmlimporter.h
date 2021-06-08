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
    void loginCorrect(bool correct);
    void findKeyAt(int pos);

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
    void checkPwd(QString check);
    void changeFindText(QString s);
    void findKey();
    void findPreviousKey();
    void findNextKey();
    void updateFindPos(int index);
    void setFindAllWord(bool allWord);
    void setFindCaseSen(bool sen);
    void setFindUseReg(bool useReg);

    QString getQtVerionString() const;
    QString getPlatform() const;
    QString getAppVersion() const;
    QString getCPPVersion() const;
    QString getBuildTime() const;
    QString getCompilerInfo() const;
#ifdef Q_OS_ANDROID
    void updateAndroidNotifier(QString msg);
#endif

private:
    QString configPath = "";
    bool autoChangeAES=false;
    QString workPath = "";
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    Kcdb *kcdb = nullptr;
    QMap<int, KeyMap> keyMap;
    QList<Estring> existsKeys;
    QString msg;
    bool autoBackupPath=false;
    quint32 keyTableRowCount = 0;
    int deletedKeysCount=0;
    QString pwPath = "";
    Estring truePwdHash;
    const Estring salt1 = Estring("15^vAd[74AC'v7456.sdO&Pv61v铸下这铁链，江东天险牢不可破");
    const Estring salt2 = Estring("離れない君といた夏のおわりゼロゼロさてんるいこおかえり");
    QString findText = "";
    int findMode = 0;
    bool findDirection = true; // =true，快捷键F3控制向下搜索，=false，快捷键F3控制向上搜索
    int keyTableFindPos = -1;
    int startPos = 0;
    bool findAllWord = false;
    Qt::CaseSensitivity findCaseSen = Qt::CaseInsensitive;
    bool findUseReg = false;

#ifdef Q_OS_ANDROID
    void initPermission();
#endif
    void initConfig();
    void initKey();
    void initKeyData();
    bool checkDb(QString dbPath = "");
    void refreshAESKey();
    void syncKeyFromMap();
    void syncKeyMapToKcdb();
    void writeCheckFile(QString checkPath);
    void saveConfig();
    bool findCheckKey() const;


#ifndef Q_OS_ANDROID
    QString parsePath(QString path);
#endif
};

#endif // QMLIMPORTER_H
