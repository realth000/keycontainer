#include "inputkeyui.h"
#include "ui_inputkeyui.h"
#include "qssinstaller.h"
#include "ui/titlebar.h"
#include <QDebug>
#include "debugshowoptions.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

// TODO: 密码验证， 格式，存储
InputKeyUi::InputKeyUi(QWidget *parent, quint32 id, KeyMap *keyMap) :
    QDialog(parent), tid(id), m_keyMap(keyMap),
    ui(new Ui::InputKeyUi)
{
    ui->setupUi(this);
    initUi();
    m_keyMap->id = tid;
    m_keyMap->disc = Estring();
    m_keyMap->account = Estring();
    m_keyMap->password = Estring();
}


InputKeyUi::~InputKeyUi()
{
    delete ui;
    delete m_keyMap;
}

void InputKeyUi::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return:
        on_inputKey_saveB_clicked();
        event->accept();
        break;
    case Qt::Key_Enter:
        on_inputKey_saveB_clicked();
        event->accept();
        break;
    default:
        break;
    }
}

// 当会触发QPushbutton的released信号时，不会触发mouseReleaseEvent
void InputKeyUi::mouseReleaseEvent(QMouseEvent *event)
{
    if(showing){
        ui->inputKey_pwdLE->setEchoMode(QLineEdit::Password);
        ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Password);
        showing = false;
        event->accept();
    }
}

void InputKeyUi::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    QssInstaller w;
    this->setStyleSheet(w.QssInstallFromFile(":/qss/stylesheet_inputkeyui.qss").arg(this->objectName()).arg("rgb(55,85,100)")
                            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                                 "alternate-background-color:rgb(55,55,55)"));
    // 标题栏样式
    ui->titleBar->setCloseIcon(":/src/close.png");
    ui->titleBar->setTitleText("添加密码");

    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(":/src/title.png");

    setTabOrder(ui->inputKey_discLE, ui->inputKey_accLE);
    setTabOrder(ui->inputKey_accLE, ui->inputKey_pwdLE);
    setTabOrder(ui->inputKey_pwdLE, ui->inputKey_conPwdLE);
    ui->inputKey_discLE->setFocus();
    ui->showPwdB->setFocusPolicy(Qt::NoFocus);

    ui->hintLabel->setVisible(false);

    ui->inputKey_pwdLE->setEchoMode(QLineEdit::Password);
    ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Password);
}

// TODO: unsafe when inputing new password;
bool InputKeyUi::checkInput()
{
    if(ui->inputKey_discLE->text() == ""){
        ui->hintLabel->setText("说明不能为空");
        ui->hintLabel->setVisible(true);
        return false;
    }
    else if(ui->inputKey_accLE->text() == ""){
        ui->hintLabel->setText("账号不能为空");
        ui->hintLabel->setVisible(true);
        return false;
    }
    else if(ui->inputKey_pwdLE->text() == ""){
        ui->hintLabel->setText("未输入密码");
        ui->hintLabel->setVisible(true);
        return false;
    }
    else if(ui->inputKey_conPwdLE->text() == ""){
        ui->hintLabel->setText("未确认密码");
        ui->hintLabel->setVisible(true);
        return false;
    }
    else if(ui->inputKey_pwdLE->text() != ui->inputKey_conPwdLE->text()){
        ui->hintLabel->setText("两次输入密码不一致");
        ui->hintLabel->setVisible(true);
        return false;
    }
    else{
        ui->hintLabel->setVisible(false);
        m_keyMap->disc = Estring(ui->inputKey_discLE->text());
        m_keyMap->account = Estring(ui->inputKey_accLE->text());
        m_keyMap->password = Estring(ui->inputKey_pwdLE->text());
        return true;
    }
}

void InputKeyUi::on_inputKey_saveB_clicked()
{
    if(checkInput()){
        emit inputFinish(true);
        this->close();
    }
}

void InputKeyUi::on_showPwdB_pressed()
{
    ui->inputKey_pwdLE->setEchoMode(QLineEdit::Normal);
    ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Normal);
    showing = true;
}
void InputKeyUi::on_showPwdB_released()
{
    ui->inputKey_pwdLE->setEchoMode(QLineEdit::Password);
    ui->inputKey_conPwdLE->setEchoMode(QLineEdit::Password);
    showing = false;
}
