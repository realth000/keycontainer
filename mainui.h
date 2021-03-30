#ifndef MAINUI_H
#define MAINUI_H

#include <QCheckBox>
#include <QMap>
#include "inputkeyui.h"
#include "login.h"
#include "kcdb.h"
#include <QPoint>

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

private:
    Kcdb *kcdb;
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
    void showPw();
    void deleteSingleKey();

    LogIn *logIn;
    Ui::MainUi *ui;
    quint32 keyTableRowCount = 0;
    QList<QCheckBox *> checkBoxItem;
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
    bool isShowing = true;

private slots:
    void showKeyTableMenu(QPoint);

    void on_clearLogBtn_clicked();
    void on_exportKeyBtn_clicked();
};
#endif // MAINUI_H
