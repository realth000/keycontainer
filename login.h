#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QCloseEvent>
#include <QKeyEvent>
#include "encryption/encrypted.h"
#include "ui/messageboxexx.h"
#include "ui/animationrefresh.h"

namespace Ui {
class LogIn;
}

class LogIn : public QDialog
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr, Estring keyFilePath = Estring(""));
    ~LogIn();
    bool getContinueStart() const;
    void setContinueStart(bool yon);

signals:
    void finish(bool r, Estring truePwdHash);
    void setKcdbKey(Estring k);

protected:
    virtual void closeEvent(QCloseEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void keyReleaseEvent(QKeyEvent *e) override;

private slots:
    void on_logInB_clicked();

private:
    void initUi();
    void readPwd();
    bool checkPwd();

    bool pwdTruth = false;
    bool continueStart = true;
    const Estring salt1 = Estring("15^vAd[74AC'v7456.sdO&Pv61v铸下这铁链，江东天险牢不可破");
    const Estring salt2 = Estring("離れない君といた夏のおわりゼロゼロさてんるいこおかえり");
    Estring pwPath = Estring("");
    Estring truePwdHash = Estring("");
    Ui::LogIn *ui;
    MessageBoxExX mb;
    PushButtonStyle *p;
};

#endif // LOGIN_H
