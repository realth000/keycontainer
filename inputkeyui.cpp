#include "inputkeyui.h"
#include "ui_inputkeyui.h"
#include "qssinstaller.h"
#include "ui/titlebar.h"
#include <QDebug>
#include "debugshowoptions.h"
#include "ui/messageboxexx.h"

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

// TODO: 密码验证， 格式，存储
InputKeyUi::InputKeyUi(QWidget *parent, quint32 id, KeyMap *keyMap, QList<Estring> discIndex) :
    QDialog(parent), tid(id), m_keyMap(keyMap), m_existsKeys(discIndex),
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

void InputKeyUi::keyPressEvent(QKeyEvent *e)
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
    caps_state ? ui->capsLockHintL->setVisible(true) : ui->capsLockHintL->setVisible(false);
    e->accept();
    return;
#endif
    }
    if((e->modifiers() == Qt::KeypadModifier && e->key() == Qt::Key_Enter)
            || (e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_Return)){
        qDebug() << "Key_Enter" << e->modifiers() << e->key();
        on_inputKey_saveB_clicked();
        e->accept();
        return;
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
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/qss/stylesheet_inputkeyui.qss").arg(this->objectName()).arg("rgb(55,85,100)")
                            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                                 "alternate-background-color:rgb(55,55,55)"));
    // 标题栏样式
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText("添加密码");

    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);

    setTabOrder(ui->inputKey_discLE, ui->inputKey_accLE);
    setTabOrder(ui->inputKey_accLE, ui->inputKey_pwdLE);
    setTabOrder(ui->inputKey_pwdLE, ui->inputKey_conPwdLE);
    ui->inputKey_discLE->setFocus();
    ui->showPwdB->setFocusPolicy(Qt::NoFocus);

    ui->hintLabel->setVisible(false);

    ui->inputKey_pwdLE->setEchoMode(QLineEdit::Password);
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
    caps_state ? ui->capsLockHintL->setVisible(true) : ui->capsLockHintL->setVisible(false);
#endif
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
        return true;
    }
}

bool InputKeyUi::checkExistence()
{
    int length = m_existsKeys.length();
    for(int i=0; i<length; i++){
        if(m_existsKeys[i].getVal().compare(ui->inputKey_discLE->text()) == 0){
            existPos = i;
            return true;
        }
    }

    return false;
}

void InputKeyUi::on_inputKey_saveB_clicked()
{
    // 输入不符合要求时返回，不保存
    if(!checkInput()){
        return;
    }
    existPos = -1;
    // 检查对应说明的密码是否已经存在，如果在，选择是否更新其账号和密码（选否则返回）
    if(checkExistence()){
        MessageBoxExX m;
        int result = m.warning("密码已存在", "已经存在与该说明相同的密码，是否将该密码及其更新为刚输入的值？", "确认更改", "不做更改");
        if(result != MessageBoxExX::Yes){
            return;
        }
    }
    m_keyMap->disc = Estring(ui->inputKey_discLE->text());
    m_keyMap->account = Estring(ui->inputKey_accLE->text());
    m_keyMap->password = Estring(ui->inputKey_pwdLE->text());
    emit inputFinish(true, existPos);
    this->close();
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
