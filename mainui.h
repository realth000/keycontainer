#ifndef MAINUI_H
#define MAINUI_H

#include <QCheckBox>
#include <QMap>
#include "inputkeyui.h"
#include "login.h"
#include "kcdb.h"

QT_BEGIN_NAMESPACE
namespace Ui {class MainUi;}
QT_END_NAMESPACE


class MainUi : public QWidget
{
    Q_OBJECT

public:
    MainUi(QWidget *parent = nullptr);
    ~MainUi();

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

private:
    Kcdb *kcdb;
    void initKeyData();
    void initUi();
    void initConfig();
    QWidget* addCheckBox();
    void keyTW_addNewRow(int rowIndex, QString disc, QString account, QString key, int rowHeight);
    void keyTW_deleteSeledtedKeys();
    void showKeyTableKeys();
    void addKey();
    // TODO: How to Refresh Aes Key?
    void refreshAESKey();
    void checkDb();


    LogIn *logIn;
    Ui::MainUi *ui;
    quint32 keyTableRowCount = 0;
    QList<QCheckBox *> checkBoxItem;
    QMap<int, KeyMap> keyMap;
    quint32 keyTW_chkBoxCheckNum;
    bool is_show_pwd = false;
    const QString pwdCharacterString = "❄❄❄❄❄❄❄";
    bool autoChangeAES = false;
    bool open = false;
    Estring truePwdHash = Estring();
    QString workPath = "";
    QString savePath = "";
    QString backupPath = "";
};
#endif // MAINUI_H
