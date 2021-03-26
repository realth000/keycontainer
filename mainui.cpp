#include "mainui.h"
#include "ui_mainui.h"
#include "qssinstaller.h"
#include "ui/titlebar.h"
#include "uistyle/proxystyle.h"
#include <QIcon>
#include <QPixmap>
#include <QBoxLayout>
#include <QScrollBar>
#include <QSettings>
#include <QCoreApplication>
#include <QProcess>
#include <QEventLoop>
#include "inputinitkeyui.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MainUi::MainUi(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainUi)
{
    QEventLoop loop;
    logIn = new LogIn();
    connect(logIn, &LogIn::finish, this, [=](bool result, Estring pwdHash){
        if(result){
            emit open2();
            truePwdHash = pwdHash;
        }
        else{
            this->close();
        }
    });
    connect(this, &MainUi::open2, &loop, &QEventLoop::quit);
//    logIn->show();
//    loop.exec();
    ui->setupUi(this);
    initUi();
    initConfig();
}

MainUi::~MainUi()
{
    delete ui;
    delete kcdb;
}

void MainUi::initKeyData()
{
    workPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    kcdb = new Kcdb(workPath);
    // TODO: check savepath before read kcdb
    kcdb->readKcdb();
    savePath = QDir::toNativeSeparators(workPath + "./pw.kcdb");
    backupPath = QDir::toNativeSeparators(workPath + "./pwbp.kcdb");
}

void MainUi::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    QssInstaller w;
    this->setStyleSheet(w.QssInstallFromFile(":/qss/stylesheet.qss").arg(this->objectName()).arg("rgb(55,85,100)")
                            .arg("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 rgb(45,45,45), stop: 1 rgb(51,51,51));"
                                 "alternate-background-color:rgb(55,55,55)"));
    // 标题栏样式
    ui->titleBar->setCloseIcon(":/src/close.png");
    ui->titleBar->setTitleText("KeyContainer - 2.0");

    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(":/src/title.png");

    // 选项卡样式
    ui->mainTabWidget->setTabText(0, "管理");
    ui->mainTabWidget->setTabText(1, "设置");
    ui->mainTabWidget->setTabPosition(QTabWidget::West);
    ui->mainTabWidget->setAttribute(Qt::WA_StyledBackground);
    ui->mainTabWidget->tabBar()->setStyle(new TabBarStyle);
    ui->mainTabWidget->setStyle(new TabWidgetStyle);

    QIcon tabIco0;
    const QPixmap tab0_1 = QPixmap(":/src/manage.png");
    const QPixmap tab0_2 = QPixmap(":/src/manage_reverse.png");
    if(!tab0_1.isNull() && !tab0_2.isNull()){
        tabIco0.addPixmap(tab0_1, QIcon::Selected, QIcon::Off);
        tabIco0.addPixmap(tab0_2, QIcon::Normal, QIcon::Off);
    }
    ui->mainTabWidget->tabBar()->setTabIcon(0, tabIco0);

    QIcon tabIco1;
    const QPixmap tab1_1 = QPixmap(":/src/config.png");
    const QPixmap tab1_2 = QPixmap(":/src/config_reverse.png");
    if(!tab1_1.isNull() && !tab1_2.isNull()){
        tabIco1.addPixmap(tab1_1, QIcon::Selected, QIcon::Off);
        tabIco1.addPixmap(tab1_2, QIcon::Normal, QIcon::Off);

    }
    ui->mainTabWidget->tabBar()->setTabIcon(1, tabIco1);

    // TextEdiit
    ui->logTE->setEnabled(false);
    ui->hintTE->setEnabled(false);

    // PushButton style
    // 注意在qss里不要对Pushbutton设置样式，包括对其parent的背景色、文字颜色的设置，否则可能导致PushButtonStyle被qss覆盖。
    QIcon addKeyIcon;
    const QPixmap pixm1 = QPixmap(":/src/addKey.png");
    if(!pixm1.isNull()){
        addKeyIcon.addPixmap(pixm1, QIcon::Normal, QIcon::Off);
    }
    ui->addKeyBtn->setIcon(addKeyIcon);


    QIcon exportKeyIcon;
    const QPixmap pixm2 = QPixmap(":/src/exportKey.png");
    if(!pixm2.isNull()){
        exportKeyIcon.addPixmap(pixm2, QIcon::Normal, QIcon::Off);
    }
    ui->exportKeyBtn->setIcon(exportKeyIcon);

    QIcon showKeyIcon;
    const QPixmap pixm3 = QPixmap(":/src/showKey.png");
    if(!pixm3.isNull()){
        showKeyIcon.addPixmap(pixm3, QIcon::Normal, QIcon::Off);
    }
    ui->showKeyBtn->setIcon(showKeyIcon);

    QIcon backupKeyIcon;
    const QPixmap pixm4 = QPixmap(":/src/backupKey.png");
    if(!pixm4.isNull()){
        backupKeyIcon.addPixmap(pixm4, QIcon::Normal, QIcon::Off);
    }
    ui->backupKeyBtn->setIcon(backupKeyIcon);

    QIcon saveKeyIcon;
    const QPixmap pixm5 = QPixmap(":/src/saveKey.png");
    if(!pixm5.isNull()){
        saveKeyIcon.addPixmap(pixm5, QIcon::Normal, QIcon::Off);
    }
    ui->saveKeyBtn->setIcon(saveKeyIcon);

    QIcon selectInverseKeyIcon;
    const QPixmap pixm6 = QPixmap(":/src/selectInverseKey.png");
    if(!pixm6.isNull()){
        selectInverseKeyIcon.addPixmap(pixm6, QIcon::Normal, QIcon::Off);
    }
    ui->selectInverseKeyBtn->setIcon(selectInverseKeyIcon);

    QIcon selectAllKeyIcon;
    const QPixmap pixm7 = QPixmap(":/src/selectAll.png");
    if(!pixm7.isNull()){
        selectAllKeyIcon.addPixmap(pixm7, QIcon::Normal, QIcon::Off);
    }
    ui->selectAllKeyBtn->setIcon(selectAllKeyIcon);

    QIcon selSelectKeyIcon;
    const QPixmap pixm8 = QPixmap(":/src/delKey.png");
    if(!pixm8.isNull()){
        selSelectKeyIcon.addPixmap(pixm8, QIcon::Normal, QIcon::Off);
    }
    ui->delSelectKeyBtn->setIcon(selSelectKeyIcon);

    QIcon saveConfigIcon;
    const QPixmap pixm9 = QPixmap(":/src/saveConfig.png");
    if(!pixm9.isNull()){
        saveConfigIcon.addPixmap(pixm9, QIcon::Normal, QIcon::Off);
    }
    ui->saveConfigBtn->setIcon(saveConfigIcon);

    QIcon restartIcon;
    const QPixmap pixm10 = QPixmap(":/src/restart.png");
    if(!pixm10.isNull()){
        restartIcon.addPixmap(pixm10, QIcon::Normal, QIcon::Off);
    }
    ui->restartProgBtn->setIcon(restartIcon);

    QIcon changeInitKeyIcon;
    const QPixmap pixm11 = QPixmap(":/src/changeInitKey.png");
    if(!pixm11.isNull()){
        changeInitKeyIcon.addPixmap(pixm11, QIcon::Normal, QIcon::Off);
    }
    ui->changeInitKeyBtn->setIcon(changeInitKeyIcon);

    QIcon changeAESKeyIcon;
    const QPixmap pixm12 = QPixmap(":/src/changeAESKey.png");
    if(!pixm12.isNull()){
        changeAESKeyIcon.addPixmap(pixm12, QIcon::Normal, QIcon::Off);
    }
    ui->changeAESKeyBtn->setIcon(changeAESKeyIcon);

    ui->addKeyBtn->setStyle(new PushButtonStyle);
    ui->backupKeyBtn->setStyle(new PushButtonStyle);
    ui->delSelectKeyBtn->setStyle(new PushButtonStyle);
    ui->exportKeyBtn->setStyle(new PushButtonStyle);
    ui->showKeyBtn->setStyle(new PushButtonStyle);
    ui->selectAllKeyBtn->setStyle(new PushButtonStyle);
    ui->selectInverseKeyBtn->setStyle(new PushButtonStyle);
//    ui->clearLogBtn->setStyle(new PushButtonStyle);
    ui->saveKeyBtn->setStyle(new PushButtonStyle);
    ui->saveConfigBtn->setStyle(new PushButtonStyle);
    ui->restartProgBtn->setStyle(new PushButtonStyle);
    ui->changeInitKeyBtn->setStyle(new PushButtonStyle);
    ui->changeAESKeyBtn->setStyle(new PushButtonStyle);

    ui->savePathLE->setReadOnly(true);
    ui->backupPathLE->setReadOnly(true);


    ui->keyTW->setColumnCount(5);
    ui->keyTW->setHorizontalHeaderLabels({"选择", "id", "说明","账户", "密码"});
    ui->keyTW->setColumnWidth(0,40);
    ui->keyTW->setColumnWidth(1,80);
    ui->keyTW->setColumnWidth(2,180);
    ui->keyTW->setColumnWidth(3,180);
    ui->keyTW->setColumnWidth(4,200);

    ui->keyTW->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->keyTW->setSelectionBehavior(QTableWidget::SelectRows);
    ui->keyTW->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->keyTW->verticalHeader()->setVisible(false);
    ui->keyTW->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->keyTW->horizontalHeader()->setHighlightSections(false);
    ui->keyTW->horizontalHeader()->setStretchLastSection(true);
    ui->keyTW->setAlternatingRowColors(true);
    // TODO: keyTW右键菜单
//    connect(ui->keyTW, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showKeyTableMenu(QPoint)), Qt::UniqueConnection);

    ui->keyTW->horizontalScrollBar()->setStyle(new HorizontalScrollBarStyle);
    ui->keyTW->verticalScrollBar()->setStyle(new VerticalScrollBarStyle);

    ui->key_checkRB->setStyle(new RadioButtonStyle);
    ui->key_clickRB->setStyle(new RadioButtonStyle);
    ui->key_doubleClickRB->setStyle(new RadioButtonStyle);
    ui->key_check_defaultRB->setStyle(new RadioButtonStyle);
    ui->key_click_defaultRB->setStyle(new RadioButtonStyle);
    ui->key_doubleClick_defaultRB->setStyle(new RadioButtonStyle);

    ui->autoChangeAESKeyChB->setStyle(new CheckBoxStyle);

}

void MainUi::initConfig()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);
    ui->savePathLE->setText(ini->value("/Path/SavePath").toString());
    ui->backupPathLE->setText(ini->value("/Path/BackupPath").toString());
    int selectMode = ini->value("/Setting/DefaultSelectMode").toUInt();
    switch(selectMode){
    case 0:
        ui->key_checkRB->setChecked(true);
        ui->key_check_defaultRB->setChecked(true);
        break;
    case 1:
        ui->key_clickRB->setChecked(true);
        ui->key_click_defaultRB->setChecked(true);
        break;
    case 2:
        ui->key_doubleClickRB->setChecked(true);
        ui->key_doubleClick_defaultRB->setChecked(true);
        break;
    default:
        ui->key_checkRB->setChecked(true);
        ui->key_check_defaultRB->setChecked(true);
    }
    autoChangeAES = ini->value("/Setting/AutoChangeAESKey").toBool();
    ui->autoChangeAESKeyChB->setChecked(autoChangeAES);
    delete ini;
}

QWidget* MainUi::addCheckBox()
{
    QCheckBox *check = new QCheckBox(this);
    check->setStyle(new CheckBoxStyle);
    checkBoxItem.append(check);
    // TODO: keyTW 的 CheckBox的select信号与选中信号的减少
    connect(check, SIGNAL(stateChanged(int)), this, SLOT(selectChecked(int)));
    QVBoxLayout *vb = new QVBoxLayout();
    vb->addStretch(1);
    vb->addWidget(check);
    QHBoxLayout *hb = new QHBoxLayout();
    hb->addStretch(1);
    hb->addLayout(vb);
    hb->addStretch(1);
    QWidget *resultQWidget = new QWidget();
    resultQWidget->setLayout(hb);
    return resultQWidget;
}

void MainUi::keyTW_addNewRow(int rowIndex, QString disc, QString account, QString key, int rowHeight)
{
    ui->keyTW->insertRow(rowIndex);
    ui->keyTW->setCellWidget(rowIndex, 0, addCheckBox());
    QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(rowIndex));
    idItem->setTextAlignment(Qt::AlignVCenter);
    ui->keyTW->setItem(rowIndex, 1, idItem);
    ui->keyTW->setItem(rowIndex, 2, new QTableWidgetItem(disc));
    if(is_show_pwd){
        // TODO: 此处应该显示账号及密码的明文，等一个取的方法
        ui->keyTW->setItem(rowIndex, 3, new QTableWidgetItem(account));
        ui->keyTW->setItem(rowIndex, 4, new QTableWidgetItem(key));
    }
    else{
        ui->keyTW->setItem(rowIndex, 3, new QTableWidgetItem(pwdCharacterString));
        ui->keyTW->setItem(rowIndex, 4, new QTableWidgetItem(pwdCharacterString));
    }

    ui->keyTW->setRowHeight(rowIndex, rowHeight);
}

void MainUi::keyTW_deleteSeledtedKeys()
{
    quint32 tmp_deleteNum = 0;
    for(quint32 i=0; i<keyTableRowCount;){
        if(checkBoxItem[i]->isChecked()){
            qDebug() << "delete" <<i;
            // 单纯的QList.removeAt并不会删除指针，用delete删除
            delete checkBoxItem[i];
            checkBoxItem.removeAt(i);
            ui->keyTW->removeRow(i);
            tmp_deleteNum++;
            keyTableRowCount--;
        }
        else if(tmp_deleteNum!=0){
            qDebug() << "update" <<i;
            // ID为行号，为保证行号与ID时刻统一，在删除时同步更新ID为新行号
            QTableWidgetItem *i1 = ui->keyTW->takeItem(i,1);
            KeyMap newKeyMapItem = keyMap[i1->text().toInt()];
            quint32 newItemId = i;
            newKeyMapItem.id = newItemId;
            keyMap.insert(newItemId, newKeyMapItem);
            keyMap.remove(i1->text().toInt());
            delete i1;
            ui->keyTW->setItem(i, 1, new QTableWidgetItem(QString::number(newItemId)));
            i++;
        }
        else{
            qDebug() << "pass" <<i;
            i++;
        }
    }
    keyTW_chkBoxCheckNum = 0;
}

void MainUi::showKeyTableKeys()
{
    if(!is_show_pwd){
        for(quint32 i=0; i<keyTableRowCount; i++){
            // TODO: 此处应该显示的账号及密码
            // 注意delete 与 takeItem是否有问题
            QTableWidgetItem* i1 = ui->keyTW->item(i,1);
            QTableWidgetItem* i3 = ui->keyTW->takeItem(i,3);
            QTableWidgetItem* i4 = ui->keyTW->takeItem(i,4);
//            ui->keyTW->setItem(i, 3, new QTableWidgetItem(decrypt(keyMap.value(i1->text().toInt()).account)));
            ui->keyTW->setItem(i, 3, new QTableWidgetItem(keyMap.value(i1->text().toInt()).account.getVal()));
            ui->keyTW->setItem(i, 4, new QTableWidgetItem(keyMap.value(i1->text().toInt()).password.getVal()));
            delete i3;
            delete i4;
        }
        is_show_pwd = true;
        ui->showKeyBtn->setText("隐藏密码");
    }
    else{
        for(quint32 i=0; i<keyTableRowCount; i++){
            delete ui->keyTW->takeItem(i,3);
            delete ui->keyTW->takeItem(i,4);
            ui->keyTW->setItem(i, 3, new QTableWidgetItem(pwdCharacterString));
            ui->keyTW->setItem(i, 4, new QTableWidgetItem(pwdCharacterString));
        }
        is_show_pwd = false;
        ui->showKeyBtn->setText("显示密码");
    }
}

void MainUi::addKey()
{
    KeyMap *k = new KeyMap;
    InputKeyUi *u = new InputKeyUi(this, keyTableRowCount, k);
    connect(u, &InputKeyUi::inputFinish, this, [=](bool result){
        if(result){
            keyTW_addNewRow(keyTableRowCount, k->disc.getVal(), k->account.getVal(), k->password.getVal(), 35);
            keyMap.insert(keyTableRowCount, KeyMap(keyTableRowCount, k->disc, k->account,k->password));
            keyTableRowCount++;
        }
    });
    connect(u, &InputKeyUi::inputFinish, u, &InputKeyUi::deleteLater);
    // INFO: 此处为何不需要析构？
//    connect(u, &InputKeyUi::destroyed, this, [=](){k->~KeyMap();});
    u->show();
}

void MainUi::refreshAESKey()
{

}

void MainUi::checkDb()
{
    QFileInfo configFileInfo(savePath+ ".chf");
    if(configFileInfo.exists()){
        QByteArray tmpMD5;
        QByteArray resultHash;
        QFile dbFile(savePath);
        dbFile.open(QIODevice::ReadOnly);
        QCryptographicHash hash1(QCryptographicHash::Keccak_512);
        hash1.addData(&dbFile);
        QString salt1 = "撒差盐可空拟中";
        hash1.addData(salt1.toUtf8());
        tmpMD5 = hash1.result().toHex();
        dbFile.close();
        QCryptographicHash hash2(QCryptographicHash::Keccak_512);
        hash2.addData(tmpMD5);
        QString salt2 = "未因若风柳起絮";
        hash2.addData(salt2.toUtf8());
        resultHash = hash2.result().toHex();
        QString hashFilePath = savePath + ".chf";
        QFile hashFile(hashFilePath);
        if(hashFile.open(QIODevice::ReadOnly)){
            QDataStream hashData(&hashFile);
            hashData.setVersion(QDataStream::Qt_5_12);
            QByteArray hashString;
            hashData >> hashString;
            hashFile.close();
            if(hashString.compare(resultHash) != 0){QMessageBox::information(NULL, tr("数据库被篡改"), tr("校验得数据库已被篡改，建议读取备份。"));};
        }
        else{
            QMessageBox::information(NULL, tr("无法校验数据库"), tr("数据库校验文件无法打开，数据库可能能够成功读取但可能已被篡改。"));
        }
    }
    else{
        QMessageBox::information(NULL, tr("数据库可能已被篡改"), tr("检测不到数据库的校验文件，数据库可能能够成功读取但可能已被篡改。"));
    }
}

void MainUi::selectCheckBox(int row, int column)
{
    Q_UNUSED(column);
    checkBoxItem[row]->isChecked() ? checkBoxItem[row]->setChecked(false) : checkBoxItem[row]->setChecked(true);
}


void MainUi::on_showKeyBtn_clicked()
{
    showKeyTableKeys();
}

void MainUi::selectChecked(int state)
{
    // TODO: 被选中的checkbox的数量变化后应更新“已选中x项”的提示。
    if(state == 0){keyTW_chkBoxCheckNum--;/* updateKeyTableStatusBar(); */}
    else if(state == 2){keyTW_chkBoxCheckNum++;/* updateKeyTableStatusBar(); */}
}

void MainUi::on_delSelectKeyBtn_clicked()
{
    keyTW_deleteSeledtedKeys();
}

void MainUi::on_addKeyBtn_clicked()
{
    addKey();
}

void MainUi::on_selectAllKeyBtn_clicked()
{
    keyTW_chkBoxCheckNum = keyTableRowCount;
    foreach(QCheckBox *QCB, checkBoxItem){
        if(!QCB->isChecked()){QCB->setChecked(true);}
    }
}

void MainUi::on_selectInverseKeyBtn_clicked()
{
    keyTW_chkBoxCheckNum = keyTableRowCount - keyTW_chkBoxCheckNum;
    foreach(QCheckBox *QCB, checkBoxItem){
        QCB->isChecked() ? QCB->setChecked(false) : QCB->setChecked(true);
    }
}

void MainUi::on_key_checkRB_clicked()
{
    if(disconnect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox)
        || disconnect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox)){

    }
}

void MainUi::on_key_clickRB_clicked()
{
    if(connect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox, Qt::UniqueConnection)
        || disconnect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox)){

    }
}

void MainUi::on_key_doubleClickRB_clicked()
{
    if(disconnect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox)
        || connect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox, Qt::UniqueConnection)){

    }
}

void MainUi::on_saveConfigBtn_clicked()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);
    ini->setValue("/Path/SavePath", ui->savePathLE->text());
    ini->setValue("/Path/BackupPath", ui->backupPathLE->text());
    if(ui->key_check_defaultRB->isChecked()){
        ini->setValue("/Setting/DefaultSelectMode", 0);
    }
    else if(ui->key_click_defaultRB->isChecked()){
        ini->setValue("/Setting/DefaultSelectMode", 1);
    }
    else{
        ini->setValue("/Setting/DefaultSelectMode", 2);
    }
    ini->setValue("/Setting/AutoChangeAESKey", autoChangeAES);
    delete ini;
}

void MainUi::on_autoChangeAESKeyChB_stateChanged(int arg1)
{
    arg1==0 ? autoChangeAES=false : autoChangeAES=true;
}

void MainUi::on_restartProgBtn_clicked()
{
    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void MainUi::on_changeInitKeyBtn_clicked()
{
    InputInitKeyUi *u = new InputInitKeyUi(this, truePwdHash);
    u->show();
}

void MainUi::on_saveKeyBtn_clicked()
{
    QFileInfo saveInfo(ui->savePathLE->text());
    QDir saveDir(saveInfo.path());
    if(saveDir.exists()){
        if(ui->autoChangeAESKeyChB->isChecked()){refreshAESKey();}
    }
}


