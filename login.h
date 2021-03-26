﻿#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QCloseEvent>
#include "encryption/encrypted.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class LogIn;
}

class LogIn : public QDialog
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr);
    ~LogIn();

signals:
    void finish(bool r, Estring truePwdHash);
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_logInB_clicked();

private:
    void initUi();
    void readPwd();
    bool checkPwd();

    bool pwdTruth = false;
    const QString salt1 = "15^vAd[74AC'v7456.sdO&Pv61v铸下这铁链，江东天险牢不可破";
    const QString salt2 = "離れない君といた夏のおわりゼロゼロさてんるいこおかえり";
    QString workPath = "";
    Estring truePwdHash = Estring();
    Ui::LogIn *ui;
};

#endif // LOGIN_H
