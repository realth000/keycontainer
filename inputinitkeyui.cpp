#include "inputinitkeyui.h"
#include "ui_inputinitkeyui.h"
#include <QCryptographicHash>
#include "qssinstaller.h"
#include <QDebug>
#include "debugshowoptions.h"
#include "commoninclude.h"

#ifdef Q_OS_WINDOWS
#include <windows.h>
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <X11/XKBlib.h>
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
// #undef those Xlib #defines that conflict with QEvent::Type enum
#endif

InputInitKeyUi::InputInitKeyUi(QWidget *parent, Estring oldPwdHash, Estring pwFilePath) :
    QDialog(parent), m_oldPwdHash(oldPwdHash), pwFilePath(pwFilePath),
    ui(new Ui::InputInitKeyUi)
{
    ui->setupUi(this);
    initUi();
}

InputInitKeyUi::~InputInitKeyUi()
{
    delete ui;
}

void InputInitKeyUi::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_CapsLock){
#ifdef Q_OS_WINDOWS
        GetKeyState(VK_CAPITAL) & 1 ? ui->capsLockHintL->setVisible(true) : ui->capsLockHintL->setVisible(false);
        e->accept();
        return;
#elif defined(Q_OS_LINUX)  && !defined(Q_OS_ANDROID)// X11 version (Linux/Unix/Mac OS X/etc...)
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
        on_inputKey_saveB_clicked();
        e->accept();
        return;
    }
    e->ignore();
}

void InputInitKeyUi::mouseReleaseEvent(QMouseEvent *event)
{
    if(showing){
        ui->inputKey_oldPwdLE->setEchoMode(QLineEdit::Password);
        ui->inputKey_newPwdLE->setEchoMode(QLineEdit::Password);
        ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Password);
        showing = false;
        event->accept();
    }
}

void InputInitKeyUi::on_inputKey_saveB_clicked()
{
    checkInput();
}


void InputInitKeyUi::on_showPwdB_pressed()
{
    ui->inputKey_oldPwdLE->setEchoMode(QLineEdit::Normal);
    ui->inputKey_newPwdLE->setEchoMode(QLineEdit::Normal);
    ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Normal);
    showing = true;
}

void InputInitKeyUi::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/qss/stylesheet_inputinitkeyui.qss").arg(this->objectName()).arg("rgb(55,85,100)")
                            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                                 "alternate-background-color:rgb(55,55,55)"));
    // 标题栏样式
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText("添加密码");

    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);

    setTabOrder(ui->inputKey_oldPwdLE, ui->inputKey_newPwdLE);
    setTabOrder(ui->inputKey_newPwdLE, ui->inputKey_conPwdLE);
    ui->inputKey_oldPwdLE->setFocus();
    ui->showPwdB->setFocusPolicy(Qt::NoFocus);

    ui->hintLabel->setVisible(false);

    ui->inputKey_oldPwdLE->setEchoMode(QLineEdit::Password);
    ui->inputKey_newPwdLE->setEchoMode(QLineEdit::Password);
    ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Password);

#ifdef Q_OS_WINDOWS
        GetKeyState(VK_CAPITAL) & 1 ? ui->capsLockHintL->setVisible(true) : ui->capsLockHintL->setVisible(false);
#elif defined(Q_OS_LINUX)  && !defined(Q_OS_ANDROID)// X11 version (Linux/Unix/Mac OS X/etc...)
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

void InputInitKeyUi::checkInput()
{
    if(ui->inputKey_oldPwdLE->text() == ""){
        ui->hintLabel->setText("未输入旧密码");
        ui->hintLabel->setVisible(true);
    }
    else if(ui->inputKey_newPwdLE->text() == ""){
        ui->hintLabel->setText("未输入密码");
        ui->hintLabel->setVisible(true);
    }
    else if(ui->inputKey_conPwdLE->text() == ""){
        ui->hintLabel->setText("未确认密码");
        ui->hintLabel->setVisible(true);
    }
    else if(ui->inputKey_newPwdLE->text() != ui->inputKey_conPwdLE->text()){
        ui->hintLabel->setText("两次输入密码不一致");
        ui->hintLabel->setVisible(true);
    }
    else if(!checkInputOldPwHash()){
        ui->hintLabel->setText("旧密码不正确");
        ui->hintLabel->setVisible(true);
    }
    else{
        ui->hintLabel->setVisible(false);
        // TODO: 写入新密码
        hashFile.setFileName(pwFilePath.getVal());
        emit writePw(Estring(ui->inputKey_newPwdLE->text()));
    }
}

bool InputInitKeyUi::checkInputOldPwHash()
{
    QByteArray tmpMD5;
    QByteArray resultHash;
    QCryptographicHash hash1(QCryptographicHash::Keccak_512);
    hash1.addData(ui->inputKey_oldPwdLE->text().toUtf8());
    hash1.addData(salt1.getVal().toUtf8());
    tmpMD5 = hash1.result().toHex();
    QCryptographicHash hash2(QCryptographicHash::Keccak_512);
    hash2.addData(tmpMD5);
    hash2.addData(salt2.getVal().toUtf8());
    resultHash = hash2.result().toHex();
    return resultHash == m_oldPwdHash.getVal() ? true : false;
}

void InputInitKeyUi::on_showPwdB_released()
{
    ui->inputKey_oldPwdLE->setEchoMode(QLineEdit::Password);
    ui->inputKey_newPwdLE->setEchoMode(QLineEdit::Password);
    ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Password);
    showing = false;
}
