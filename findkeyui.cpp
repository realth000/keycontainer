#include "findkeyui.h"
#include "ui_findkeyui.h"
#include "ui/titlebar.h"
#include "qssinstaller.h"
#include <QDebug>
#include "uistyle/proxystyle.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

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

void FindKeyUi::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    QssInstaller w;
    this->setStyleSheet(w.QssInstallFromFile(":/qss/stylesheet_findkeyui.qss").arg(this->objectName()).arg("rgb(55,85,100)")
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

    ui->findPreBtn->setStyle(new PushButtonStyle);
    ui->findNextBtn->setStyle(new PushButtonStyle);

}

void FindKeyUi::on_findPreBtn_clicked()
{
    // 注意要先setDisable，再emit查找信号，否则大概率出现先找完，setEnable再setDisable的问题
    freezeFindBtn();
    emit findTextPrevious();
}

void FindKeyUi::on_findNextBtn_clicked()
{
    freezeFindBtn();
    emit findTextNext();
}

void FindKeyUi::on_findKeywordLE_textChanged(const QString &arg1)
{
    emit changeFindText(arg1);
}
