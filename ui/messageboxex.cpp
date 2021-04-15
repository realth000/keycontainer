#include "messageboxex.h"
#include "ui_messageboxex.h"
#include "ui/titlebar.h"
#include "commoninclude.h"
#include "qssinstaller.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MessageBoxEx::MessageBoxEx(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBoxEx)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());

    // 标题栏样式
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText(TITLEBAR_TITLETEXT);
    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMinAndMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);

    // 样式
    QssInstaller w;
    this->setStyleSheet(w.QssInstallFromFile(":/qss/stylesheet_messageboxex.qss").arg(this->objectName()).arg("rgb(55,85,100)")
            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                 "alternate-background-color:rgb(55,55,55)"));
    ui->infoTE->setReadOnly(true);

}

MessageBoxEx::~MessageBoxEx()
{
    delete ui;
}

void MessageBoxEx::information(QString titleText, QString text, QString buttonText)
{
    ui->titleBar->setTitleText(titleText);
    QMessageBox tmb;
    tmb.setIcon(QMessageBox::Information);
    QPixmap t = QPixmap(tmb.iconPixmap());
    t.scaled(ui->iconL->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->iconL->setScaledContents(true);
    ui->iconL->setPixmap(t);
    ui->infoTE->setText(text);
//    ui->infoTE->setAlignment(Qt::AlignHCenter);
    ui->button->setText(buttonText);
    ui->button2->setVisible(false);
    connect(ui->button, &QPushButton::clicked, this, &MessageBoxEx::close);
    this->exec();
}

int MessageBoxEx::warning(QString titleText, QString text, QString yesText, QString noText)
{
    ui->titleBar->setTitleText(titleText);
    QMessageBox tmb;
    tmb.setIcon(QMessageBox::Warning);
    QPixmap t = QPixmap(tmb.iconPixmap());
    t.scaled(ui->iconL->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->iconL->setScaledContents(true);
    ui->iconL->setPixmap(t);
    ui->infoTE->setText(text);
//    ui->infoTE->setAlignment(Qt::AlignHCenter);
    ui->button->setText(yesText);
    ui->button2->setText(noText);
    connect(ui->button, &QPushButton::clicked, this, &MessageBoxEx::resultToYes);
    connect(ui->button2, &QPushButton::clicked, this, &MessageBoxEx::resultToNo);
    this->exec();
    return result;
}

int MessageBoxEx::question(QString titleText, QString text, QString yesText, QString noText)
{
    ui->titleBar->setTitleText(titleText);
    QMessageBox tmb;
    tmb.setIcon(QMessageBox::Question);
    QPixmap t = QPixmap(tmb.iconPixmap());
    t.scaled(ui->iconL->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->iconL->setScaledContents(true);
    ui->iconL->setPixmap(t);
    ui->infoTE->setText(text);
    ui->button->setText(yesText);
    ui->button2->setText(noText);
    connect(ui->button, &QPushButton::clicked, this, &MessageBoxEx::resultToYes);
    connect(ui->button2, &QPushButton::clicked, this, &MessageBoxEx::resultToNo);
    this->exec();
    return result;
}

void MessageBoxEx::resultToYes()
{
    this->result = Yes;
    this->close();
}

void MessageBoxEx::resultToNo()
{
    this->result = No;
    this->close();
}
