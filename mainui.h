#ifndef MAINUI_H
#define MAINUI_H

#include <QCheckBox>
#include <QMap>
#include "inputkeyui.h"
#include "login.h"
#include "kcdb.h"
#include <QPoint>
#include "ui/messageboxex.h"
#include <QApplication>
#include "findkeyui.h"

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

signals:
    void open2();
    void mouseReleasedSig(bool pos);
    void findKeyOnRow(int row);
    void sendLogLText(QString s) const;
    void clearLogL();
    void unfreezeFindBtn() const;

protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *e) override;

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
    void on_backupKeysBtn_clicked();
    void on_about_aboutQtB_clicked();
    void on_findKeyBtn_clicked();
    void findNextKey();
    void findPreviousKey();
    void changeFindMode(int mode);
    void changeFindText(QString s);
    void countAll() const;

private:
    void initKeyData();
    void initUi();
    void initConfig();
    QWidget* addCheckBox();
    void keyTW_addNewRow(int rowIndex, Estring disc, Estring account, Estring key, int rowHeight);
    void keyTW_deleteSeledtedKeys();
    void showKeyTableKeys();
    void addKey();
    // TODO: How to Refresh Aes Key?
    void refreshAESKey();
    bool checkDb();
    void syncKeyFromMap();
    void syncKeyMapToKcdb();
    void refreshKeyTW();
    void writeCheckFile(QString checkPath);
    void showAcPw();
    void showAc();
    void showPw();
    void deleteSingleKey();

    Kcdb *kcdb = nullptr;
    LogIn *logIn = nullptr;
    Ui::MainUi *ui;
    quint32 keyTableRowCount = 0;
    int keyTableFindPos = 0;
    int startPos = 0;
    QList<QCheckBox *> checkBoxItem;
    QList<Estring> discQuickIndex;
    QMap<int, KeyMap> keyMap;
    quint32 keyTW_chkBoxCheckNum=0;
    bool is_show_pwd = false;
    const QString pwdCharacterString = "❄❄❄❄❄❄❄";
    bool autoChangeAES = false;
    bool open = false;
    Estring truePwdHash = Estring();
    QString workPath = "";
    QString savePath = "/pw.kcdb";
    QString backupPath = "/pwbp.kcdb";
    int rightClickSelectedItemRow = false;
    bool isAcountShowing = false;
    bool isKeyShowing = false;
    MessageBoxEx mb;
    FindKeyUi *fkui = nullptr;
    const QList<QString> eventFilterNames = {"QPushButton", "QCheckBox", "QWidgetWindow", "QTabBar", "QRadioButton", "QWidget", "TitleBar", "QMenu"};
    int findMode = 0;
    bool findDirection = true; // =true，快捷键F3控制向下搜索，=false，快捷键F3控制向上搜索
    QString findText = "";
};
#endif // MAINUI_H
