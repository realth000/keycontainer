﻿#include "login.h"
#include "ui_login.h"
#include "qssinstaller.h"
#include "ui/titlebar.h"
#include <QFileInfo>
#include <QCoreApplication>
#include "commoninclude.h"
#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include "debugshowoptions.h"

//83 69 91
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifdef Q_OS_WINDOWS
#include <windows.h>
#else
#include <X11/XKBlib.h>
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
// #undef those Xlib #defines that conflict with QEvent::Type enum
#endif

LogIn::LogIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);
    initUi();

    pwPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() +  "/login.ec");
    readPwd();
    connect(ui->refreshAR, &AnimationRefresh::stopped, this, [this](){ui->logInB->setVisible(true);ui->logInB->setEnabled(true);});
}

LogIn::~LogIn()
{
    delete ui;
}

bool LogIn::getContinueStart() const
{
    return this->continueStart;
}

void LogIn::setContinueStart(bool yon)
{
    this->continueStart = yon;
}

void LogIn::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    if(!continueStart){
        emit finish(false, truePwdHash);
        return;
    }
    emit finish(true, truePwdHash);
    e->accept();
}

void LogIn::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_CapsLock){
#ifdef Q_OS_WINDOWS
        GetKeyState(VK_CAPITAL) & 1 ? ui->capsLockHintL->setVisible(true) : ui->capsLockHintL->setVisible(false);
        e->accept();
        return;
#else // X11 version (Linux/Unix/Mac OS X/etc...)
    Display *d = XOpenDisplay((char*)0);
    bool caps_state = false;
    if (d) {
        unsigned n;
        XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
        caps_state = (n & 0x01) == 1;
    }
    caps_state ? ui->capslockHintL->setVisible(true) : ui->capslockHintL->setVisible(false);
    e->accept();
    return;
#endif
    }
    if((e->modifiers() == Qt::KeypadModifier && e->key() == Qt::Key_Enter)
            || (e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_Return)){
        if(!ui->logInB->isEnabled()){
            e->ignore();
            return;
        }
        on_logInB_clicked();
        e->accept();
        return;
    }
    e->ignore();
}

void LogIn::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/qss/stylesheet_login.qss").arg(this->objectName()).arg("rgb(55,85,100)")
                            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                                 "alternate-background-color:rgb(55,55,55)").arg(PUSHBUTTON_DISABLE_COLOR));
    // 标题栏样式
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText(TITLEBAR_TITLETEXT);

    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);

    ui->lineEdit->setEchoMode(QLineEdit::Password);
    ui->lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->warnL->setVisible(false);
    ui->backgroundLabel->setPixmap(QPixmap(":/src/initUiBackground.png"));
    ui->lineEdit->setFocus();
    ui->logInB->setFocusPolicy(Qt::ClickFocus);

#ifdef Q_OS_WINDOWS
        GetKeyState(VK_CAPITAL) & 1 ? ui->capsLockHintL->setVisible(true) : ui->capsLockHintL->setVisible(false);
#else // X11 version (Linux/Unix/Mac OS X/etc...)
    Display *d = XOpenDisplay((char*)0);
    bool caps_state = false;
    if (d) {
        unsigned n;
        XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
        caps_state = (n & 0x01) == 1;
    }
    caps_state ? ui->capslockHintL->setVisible(true) : ui->capslockHintL->setVisible(false);
#endif
}

void LogIn::readPwd()
{
    QFileInfo fileInfo(pwPath);
    if(!fileInfo.exists()){
        mb.information("无法启动", "密码文件丢失，无法启动。", " 退出 ");
//        emit finish(false, Estring(""));
        continueStart = false;
        return;
    }
    QFile hashFile(pwPath);
    if(!hashFile.open(QIODevice::ReadOnly)){
        mb.information("无法读取启动密码", "密码文件可能被其他程序占用。", " 退出 ");
//        emit finish(false, Estring(""));
        continueStart = false;
        return;
    }
    QDataStream hashData(&hashFile);
    QByteArray hashString;
    hashData >> hashString;
    hashFile.close();
    if(hashString == ""){
        mb.information("密码错误", "检测到密码为空。", " 退出 ");
//        emit finish(false, Estring(""));
        continueStart = false;
        return;
    }
    truePwdHash = Estring(hashString);

}

bool LogIn::checkPwd()
{
    QByteArray tmpMD5;
    QByteArray resultHash;
    QCryptographicHash hash1(QCryptographicHash::Keccak_512);
    if(ui->lineEdit->text() == ""){
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();
        return false;
    }
    hash1.addData(ui->lineEdit->text().toUtf8());
    hash1.addData(salt1.getVal().toUtf8());
    tmpMD5 = hash1.result().toHex();
    QCryptographicHash hash2(QCryptographicHash::Keccak_512);
    hash2.addData(tmpMD5);
    hash2.addData(salt2.getVal().toUtf8());
    resultHash = hash2.result().toHex();
    if(resultHash == truePwdHash.getVal()){
        return true;
    }
    else{
        ui->warnL->setVisible(true);
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();
        return false;
    }
}

void LogIn::on_logInB_clicked()
{
    if(checkPwd()){
        pwdTruth = true;
//        emit finish(true, truePwdHash);
        continueStart = true;
        this->close();
    }
    else{
        ui->warnL->setVisible(true);
        ui->logInB->setVisible(false);
        ui->logInB->setEnabled(false);
        ui->refreshAR->start(2591);
    }
}
