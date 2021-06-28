#ifndef MAINUI_H
#define MAINUI_H

#include <QCheckBox>
#include <QMap>
#include "inputkeyui.h"
#include "login.h"
#include "kcdb.h"
#include <QPoint>
#include "ui/messageboxexx.h"
#include <QApplication>
#include "findkeyui.h"
//#include <QCloseEvent>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {class MainUi;}
QT_END_NAMESPACE

class MainUi : public QWidget
{
    Q_OBJECT

public:
    MainUi(QWidget *parent = nullptr);
    ~MainUi();
    bool loginCorrent = false;

public slots:
    void log(QString log);
    void switchToRow(int row);
    void writeInitPw(Estring p);

signals:
    void open2();
    void mouseReleasedSig(bool pos);
    void findKeyOnRow(int row);
    void sendLogLText(QString s);
    void clearLogL();
    void unfreezeFindBtn();
    void writeInitPwSuccess();
//protected:
//    void closeEvent(QCloseEvent *event) override;

protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
//    virtual void keyReleaseEvent(QKeyEvent *e) override;

private slots:
    void on_showKeyBtn_clicked();
    void selectChecked(int);
    void on_delSelectKeyBtn_clicked();
    void on_addKeyBtn_clicked();
    void on_selectAllKeyBtn_clicked();
    void on_selectInverseKeyBtn_clicked();
    void on_key_checkRB_clicked();
    void selectCheckBox(int row, int column);
    void on_key_clickRB_clicked();
    void on_key_doubleClickRB_clicked();
    void on_saveConfigBtn_clicked();
    void on_autoChangeAESKeyChB_stateChanged(int arg1);
    void on_restartProgBtn_clicked();
    void on_changeInitKeyBtn_clicked();
    void on_saveKeyBtn_clicked();
    void on_backupKeyBtn_clicked();
    void on_selectSavePathBtn_clicked();
    void on_selectBackupPathBtn_clicked();
    void on_changeAESKeyBtn_clicked();
    void showKeyTableMenu(QPoint);
    void on_clearLogBtn_clicked();
    void on_exportKeyBtn_clicked();
    void on_backupDataKeyBtn_clicked();
    void on_about_aboutQtB_clicked();
    void on_findKeyBtn_clicked();
    bool findCheckKey();
    void findNextKey();
    void findPreviousKey();
    void changeFindMode(int mode);
    void changeFindText(QString s);
    void countAll();
    void on_importKeysBtn_clicked();
    void on_keyTW_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_autoBackupPathChB_stateChanged(int arg1);
    void lockApp();
    void on_lockAppBtn_clicked();
    void appStateChanged(Qt::ApplicationState state);
    void on_lockAppTimingSB_currentTextChanged(const QString &arg1);

private:
    void initKeyData();
    void initUi();
    void initConfig();
    QWidget* addCheckBox(int height);
    void keyTW_addNewRow(int rowIndex, Estring disc, Estring account, Estring key, int rowHeight, bool show=true);
    void keyTW_deleteSeledtedKeys();
    void showKeyTableKeys();
    void addKey();
    // TODO: How to Refresh Aes Key?
    void refreshAESKey();
    bool checkDb(QString dbPath);
    void syncKeyFromMap();
    void syncKeyMapToKcdb();
    void refreshKeyTW();
    void writeCheckFile(QString checkPath);
    void showAcPw();
    void showAc();
    void showPw();
    void deleteSingleKey();
    bool setKcdbKey();

    Kcdb *kcdb = nullptr;
    LogIn *logIn = nullptr;
    Ui::MainUi *ui;
    quint32 keyTableRowCount = 0;
    int keyTWCurrentRow = -1;
    int keyTableFindPos = 0;
    int startPos = 0;
    QList<QCheckBox *> checkBoxItem;
    QList<Estring> discQuickIndex;
    QMap<int, KeyMap> keyMap;
    quint32 keyTW_chkBoxCheckNum=0;
    bool is_show_pwd = false;
    const QString pwdCharacterString = "❄❄❄❄❄❄❄";
    bool autoChangeAES = false;
    Estring truePwdHash = Estring();
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    int rightClickSelectedItemRow = false;
    bool isAcountShowing = false;
    bool isKeyShowing = false;
    MessageBoxExX mb;
    FindKeyUi *fkui = nullptr;
    const QList<QString> eventFilterNames = {"QPushButton", "QCheckBox", "QWidgetWindow", "QTabBar", "QRadioButton", "QWidget", "TitleBar", "QMenu"};
    int findMode = 0;
    bool findDirection = true; // =true，快捷键F3控制向下搜索，=false，快捷键F3控制向上搜索
    QString findText = "";
    bool enableKeyTWContextMenuSearch = false; // 当要搜索的文本为空时，禁用表格右键菜单中的搜索
    int keysShowsNumber = 0; // 记录账户\密码显示的数量，对应控制显示\隐藏密码按钮
    bool autoBackupPath=false; // 跳过选择备份目录的选项
    const Estring salt1 = Estring("15^vAd[74AC'v7456.sdO&Pv61v铸下这铁链，江东天险牢不可破");
    const Estring salt2 = Estring("離れない君といた夏のおわりゼロゼロさてんるいこおかえり");
    bool restart=false;
    QEventLoop loginLockLoop;
    QString appPath="";
    QString currentPath="";
    bool onSaveKCDB=false;
    bool onBackupKCDB=false;
    Estring tmpPwdHash;
    QTimer timeLocker;
    QMap<QString, int> timeLockerTimingMap;
    int timeLockerTiming=300000;

};
#endif // MAINUI_H
