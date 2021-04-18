#ifndef INPUTINITKEYUI_H
#define INPUTINITKEYUI_H

#include <QDialog>
#include "encryption/encrypted.h"
#include <QFile>
#include <QKeyEvent>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class InputInitKeyUi;
}

class InputInitKeyUi : public QDialog
{
    Q_OBJECT

public:
    explicit InputInitKeyUi(QWidget *parent = nullptr, Estring oldPwdHash = Estring(), Estring pwFilePath = Estring());
    ~InputInitKeyUi();
signals:
    void changedPw(QString s);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_inputKey_saveB_clicked();
    void on_showPwdB_pressed();
    void on_showPwdB_released();

private:
    void initUi();
    void checkInput();
    bool checkInputOldPwHash();
    const Estring salt1 = Estring("15^vAd[74AC'v7456.sdO&Pv61v铸下这铁链，江东天险牢不可破");
    const Estring salt2 = Estring("離れない君といた夏のおわりゼロゼロさてんるいこおかえり");
    const Estring m_oldPwdHash;
    const Estring pwFilePath;
    Ui::InputInitKeyUi *ui;
    bool showing = false;
    QFile hashFile;
};

#endif // INPUTINITKEYUI_H
