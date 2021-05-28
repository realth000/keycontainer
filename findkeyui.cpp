#include "findkeyui.h"
#include "ui_findkeyui.h"
#include "ui/titlebar.h"
#include "qssinstaller.h"
#include <QDebug>
#include "uistyle/proxystyle.h"

FindKeyUi::FindKeyUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindKeyUi)
{
    ui->setupUi(this);
    initUi();
}

FindKeyUi::~FindKeyUi()
{
    delete ui;
}

void FindKeyUi::setInputFocus()
{
    // 先setActiveWindow，再setFocus才能设置好光标
    QApplication::setActiveWindow(this);
    ui->findKeywordLE->setFocus();
    ui->findKeywordLE->setCursorPosition(ui->findKeywordLE->text().size());
}

bool FindKeyUi::isFindAllWord() const
{
    return this->findAllWord;
}

Qt::CaseSensitivity FindKeyUi::isCaseSen() const
{
    return this->findCaseSen;
}

void FindKeyUi::setTransparency(bool pos)
{
    if(pos){
//        qDebug() << "FindKeyUi: setTransparency lost focus";
        this->setWindowOpacity(0.666);
    }
    else{
//        qDebug() << "FindKeyUi: setTransparency get focus";
        this->setWindowOpacity(1);
    }
}

void FindKeyUi::setLogLText(QString s)
{
    ui->logL->setText(s);
}

void FindKeyUi::clearLogL()
{
    ui->logL->setText("");
}

void FindKeyUi::freezeFindBtn() const
{
    ui->findPreBtn->setEnabled(false);
    ui->findNextBtn->setEnabled(false);
}

void FindKeyUi::unfreezeFindBtn() const
{
    ui->findPreBtn->setEnabled(true);
    ui->findNextBtn->setEnabled(true);
    //    qDebug() << "unfreeeze" << ui->findPreBtn->isEnabled() << ui->findNextBtn->isEnabled();
}

bool FindKeyUi::isFinBtnFreezed() const
{
    if((!ui->findPreBtn->isEnabled()) || (!ui->findNextBtn->isEnabled())){
        return true;
    }
    else{
        return false;
    }
}

void FindKeyUi::keyPressEvent(QKeyEvent *e)
{
    // 键盘按键按下事件中，处理打开FindKeyUi与否
    // 用press而不用release的原因是，release的触发很慢且组合键不灵敏，，且无法按住Modifier多次触发快捷键，会显得快捷键很难用
    //  保持与FindKeyUi中的keyPressEvent相同
    // F3向前或向后搜索
    if(e->key() == Qt::Key_F3 || e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter){

            //如果正在搜索中（被冻结），什么也不做
            if(this->isFinBtnFreezed()){
                e->accept();
                return;
            }
            else {
                // 判断搜索方向，执行一次搜索，先freeze再搜索，防止按快捷键多次搜索或者先unfreeze再freeze
                findDirection==false ? on_findPreBtn_clicked() : on_findNextBtn_clicked();
                e->accept();
                return;
            }

    }
    e->ignore();
}

void FindKeyUi::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/qss/stylesheet_findkeyui.qss").arg(this->objectName()).arg("rgb(55,85,100)")
                            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                                 "alternate-background-color:rgb(55,55,55)"));

    // 标题栏样式
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText("查找密码");

    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMinAndMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);
    ui->titleBar->setParent(this);
    ui->findPreBtn->setFocusPolicy(Qt::NoFocus);
    ui->findNextBtn->setFocusPolicy(Qt::NoFocus);
    ui->countBtn->setFocusPolicy(Qt::NoFocus);
    ui->findAllWordChB->setFocusPolicy(Qt::NoFocus);
    ui->findCaseSensitivityChB->setFocusPolicy(Qt::NoFocus);
    ui->useRegChB->setFocusPolicy(Qt::NoFocus);
    ui->findKeywordLE->setFocusPolicy(Qt::StrongFocus);
    ui->findKeywordLE->setFocus();
//    connect(ui->findKeywordLE, &QLineEdit::textChanged, this, &FindKeyUi::findText, Qt::UniqueConnection);

    QIcon findPreBtn;
    const QPixmap pixm1 = QPixmap(":/src/arrow_left_30.png");
    if(!pixm1.isNull()){
        findPreBtn.addPixmap(pixm1, QIcon::Normal, QIcon::Off);
    }
    ui->findPreBtn->setIcon(findPreBtn);

    QIcon findNextBtn;
    const QPixmap pixm2 = QPixmap(":/src/arrow_right_30.png");
    if(!pixm2.isNull()){
        findNextBtn.addPixmap(pixm2, QIcon::Normal, QIcon::Off);
    }
    ui->findNextBtn->setIcon(findNextBtn);
    PushButtonStyle *p = new PushButtonStyle;
    ui->findPreBtn->setStyle(p);
    ui->findNextBtn->setStyle(p);
    CheckBoxStyle *c = new CheckBoxStyle;
    ui->useRegChB->setStyle(new CheckBoxStyle);
    ui->findCaseSensitivityChB->setStyle(c);
    ui->findAllWordChB->setStyle(c);
}

void FindKeyUi::on_findPreBtn_clicked()
{
    // 注意要先setDisable，再emit查找信号，否则大概率出现先找完，setEnable再setDisable的问题
    findDirection=false;
    freezeFindBtn();
    emit findTextPrevious();
}

void FindKeyUi::on_findNextBtn_clicked()
{
    findDirection=true;
    freezeFindBtn();
    emit findTextNext();
}

void FindKeyUi::on_findKeywordLE_textChanged(const QString &arg1)
{
    emit changeFindText(arg1);
}

void FindKeyUi::on_countBtn_clicked()
{
    freezeFindBtn();
    emit countAll();
}

void FindKeyUi::on_useRegChB_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->findAllWordChB->setEnabled(false);
        ui->findCaseSensitivityChB->setEnabled(false);
    }
    else{
        ui->findAllWordChB->setEnabled(true);
        ui->findCaseSensitivityChB->setEnabled(true);
    }
    emit findKeyUseRegSig(arg1);
}

void FindKeyUi::on_findAllWordChB_stateChanged(int arg1)
{
    if(arg1 == 2){
        this->findAllWord = true;
        ui->useRegChB->setEnabled(false);
    }
    else{
        this->findAllWord = false;
        if(!ui->findCaseSensitivityChB->isChecked()){
            ui->useRegChB->setEnabled(true);
        }
    }
}

void FindKeyUi::on_findCaseSensitivityChB_stateChanged(int arg1)
{
    if(arg1 == 2){
        this->findCaseSen = Qt::CaseSensitive;
        ui->useRegChB->setEnabled(false);
    }
    else{
        this->findCaseSen = Qt::CaseInsensitive;
        if(!ui->findAllWordChB->isChecked()){
            ui->useRegChB->setEnabled(true);
        }
    }
}
