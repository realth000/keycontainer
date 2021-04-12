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
#include <QTime>
#include "debugshowoptions.h"
#include <QFileDialog>
#include <QMenu>
#include <QTimer>

//#include <QWaylandObject>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MainUi::MainUi(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainUi)
{
    workPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
//    qDebug()<< "work path:"<< workPath;

    QEventLoop loop;
    logIn = new LogIn();
    connect(logIn, &LogIn::finish, this, [=](bool result, Estring pwdHash){
        if(result){
            truePwdHash = pwdHash;
            loginCorrent = true;
        }
        emit open2();
    });
    connect(this, &MainUi::open2, &loop, &QEventLoop::quit);
    if(logIn->getContinueStart()){
        logIn->setContinueStart(false);
        logIn->show();
        loop.exec();
    }

    if(!loginCorrent){
        return;
    }
    ui->setupUi(this);
    initUi();
    initConfig();
    initKeyData();

    // 安装filter
    QApplication::instance()->installEventFilter(this);
}

MainUi::~MainUi()
{
    // TODO: 为什么*ui不需要初始化，且即使初始化为nullptr在此也不是nullptr？
    // 可能ui的parent是
    QApplication::instance()->removeEventFilter(this);;
    if(ui==nullptr){qDebug() << "nullptr *ui";}
    delete ui;
    delete kcdb;
    delete fkui;
}

void MainUi::log(QString log)
{
    ui->logTE->append( QDateTime::currentDateTime().toString("HH:mm:ss")+ " " + log);
}

bool MainUi::eventFilter(QObject *o, QEvent *e)
{
   QMouseEvent *mouseReleased = reinterpret_cast<QMouseEvent *>(e);
    if(mouseReleased != nullptr){
        // 鼠标左键释放事件中，处理FindKeyUi透明与否
        // 自然地就是，当前tabWidget不是表格那个的时，怎么点也都是透明状态
        if(!fkui->isVisible()){
            return false;
        }
        if(mouseReleased->type() == QEvent::MouseButtonRelease){
    //        qDebug() << mouseReleased->type();
            if(!fkui->geometry().contains(mouseReleased->globalPos()) && this->geometry().contains(mouseReleased->globalPos())){
                // 设置透明
                emit mouseReleasedSig(true);
            }
            else{
                // 设置不透明
                emit mouseReleasedSig(false);
            }
    //        qDebug() << o->objectName() << o->metaObject()->className();

            // FIXME: 多次触发？
            QString name = QString(o->metaObject()->className());
            if(eventFilterNames.contains(name)){
                return false;
            }
            else if(name.contains("LineEdit")){
                QLineEdit *e = reinterpret_cast<QLineEdit *>(o);
                if(e!=nullptr && !e->isReadOnly() && e->isEnabled()){
                    return false;
                }
                else{
    //                qDebug() << "EventFilter true: LineEdit";
                    return true;
                }
            }
            else if(name.contains("TextEdit")){
                QTextEdit *e = reinterpret_cast<QTextEdit *>(o);
                if(e!=nullptr && !e->isReadOnly() && e->isEnabled()){
                    return false;
                }
                else{
    //                qDebug() << "EventFilter true: TextEdit";
                    return true;
                }
            }
            else{
    //            qDebug() << "EventFilter true: final else";
                return true;
            }
        }
        return false;
    }

    return false;
}

void MainUi::keyPressEvent(QKeyEvent *e)
{
    // 键盘按键按下事件中，处理打开FindKeyUi与否
    // 用press而不用release的原因是，release的触发很慢且组合键不灵敏，，且无法按住Modifier多次触发快捷键，会显得快捷键很难用
    //  保持与FindKeyUi中的keyPressEvent相同
    if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_F){
        if(ui->mainTabWidget->currentIndex()!=0){return;}
        if(fkui->isVisible()){
            fkui->close();
            qDebug() << "catch Ctrl + F: close fkUi";
            e->accept();
            return;
        }
        else{
            on_findKeyBtn_clicked();
            qDebug() << "catch Ctrl + F: open fkUi";
            e->accept();
            return;
        }

    }
    // F3向前或向后搜索
    if(e->key() == Qt::Key_F3 || e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter){
        if(!fkui->isVisible()){
            e->ignore();
            return;
        }
        else{
            //如果正在搜索中（被冻结），什么也不做
            if(fkui->isFinBtnFreezed()){
                e->ignore();
                return;
            }
            else {
                qDebug() << "get" <<e->key();
                // 判断搜索方向，执行一次搜索，先freeze再搜索，防止按快捷键多次搜索或者先unfreeze再freeze
                fkui->freezeFindBtn();
                findDirection==false ? findPreviousKey() : findNextKey();
                e->accept();
                return;
            }
        }
    }
    e->ignore();
}

void MainUi::initKeyData()
{
    if(!(QFileInfo(savePath)).exists()){
        log("未找到数据文件");
        return;
    }

    if(!checkDb()){
        log("数据校验未通过，拒绝读取数据");
        return;
    }
    if(kcdb->readKcdb()){
        syncKeyFromMap();
        refreshKeyTW();
    }

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
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText(TITLEBAR_TITLETEXT);
    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBar::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);

    // 选项卡样式
    ui->mainTabWidget->setTabText(0, "管理");
    ui->mainTabWidget->setTabText(1, "设置");
    ui->mainTabWidget->setTabText(2, "关于");
    ui->mainTabWidget->setTabPosition(QTabWidget::West);
    ui->mainTabWidget->setAttribute(Qt::WA_StyledBackground);
    ui->mainTabWidget->tabBar()->setStyle(new TabBarStyle);
    ui->mainTabWidget->setStyle(new TabWidgetStyle);
    ui->mainTabWidget->setCurrentIndex(0);

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

    QIcon tabIco2;
    const QPixmap tab2_1 = QPixmap(":/src/about.png");
    const QPixmap tab2_2 = QPixmap(":/src/about_reverse.png");
    if(!tab2_1.isNull() && !tab2_2.isNull()){
        tabIco1.addPixmap(tab2_1, QIcon::Selected, QIcon::Off);
        tabIco1.addPixmap(tab2_2, QIcon::Normal, QIcon::Off);

    }
    ui->mainTabWidget->tabBar()->setTabIcon(2, tabIco1);

    // TextEdiit
    ui->logTE->setReadOnly(true);
    ui->logTE->horizontalScrollBar()->setStyle(new HorizontalScrollBarStyle);
    ui->logTE->verticalScrollBar()->setStyle(new VerticalScrollBarStyle);
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

    QIcon backupKeysIcon;
    const QPixmap pixm13 = QPixmap(":/src/backupKeys.png");
    if(!pixm13.isNull()){
        backupKeysIcon.addPixmap(pixm13, QIcon::Normal, QIcon::Off);
    }
    ui->backupKeysBtn->setIcon(backupKeysIcon);

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

    QIcon findKeyIcon;
    const QPixmap pixm14 = QPixmap(":/src/findKey_reverse.png");
    if(!pixm14.isNull()){
        findKeyIcon.addPixmap(pixm14, QIcon::Normal, QIcon::Off);
    }
    ui->findKeyBtn->setIcon(findKeyIcon);

    ui->addKeyBtn->setStyle(new PushButtonStyle);
    ui->backupKeyBtn->setStyle(new PushButtonStyle);
    ui->backupKeysBtn->setStyle(new PushButtonStyle);
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
    ui->findKeyBtn->setStyle(new PushButtonStyle);

    ui->addKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->backupKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->backupKeysBtn->setFocusPolicy(Qt::NoFocus);
    ui->delSelectKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->exportKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->showKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->selectAllKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->selectInverseKeyBtn->setFocusPolicy(Qt::NoFocus);
    //    ui->clearLogBtn->setFocusPolicy(Qt::NoFocus);
    ui->saveKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->saveConfigBtn->setFocusPolicy(Qt::NoFocus);
    ui->restartProgBtn->setFocusPolicy(Qt::NoFocus);
    ui->changeInitKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->changeAESKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->findKeyBtn->setFocusPolicy(Qt::NoFocus);

    ui->savePathLE->setReadOnly(true);
    ui->backupPathLE->setReadOnly(true);


    ui->keyTW->setColumnCount(5);
    ui->keyTW->setHorizontalHeaderLabels({"选择", "id", "说明","账户", "密码"});
    ui->keyTW->setColumnWidth(0,40);
    ui->keyTW->setColumnWidth(1,60);
    ui->keyTW->setColumnWidth(2,150);
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
    connect(ui->keyTW, &QTableWidget::customContextMenuRequested, this, &MainUi::showKeyTableMenu, Qt::UniqueConnection);

    ui->keyTW->horizontalScrollBar()->setStyle(new HorizontalScrollBarStyle);
    ui->keyTW->verticalScrollBar()->setStyle(new VerticalScrollBarStyle);

    ui->key_checkRB->setStyle(new RadioButtonStyle);
    ui->key_clickRB->setStyle(new RadioButtonStyle);
    ui->key_doubleClickRB->setStyle(new RadioButtonStyle);
    ui->key_check_defaultRB->setStyle(new RadioButtonStyle);
    ui->key_click_defaultRB->setStyle(new RadioButtonStyle);
    ui->key_doubleClick_defaultRB->setStyle(new RadioButtonStyle);

    ui->autoChangeAESKeyChB->setStyle(new CheckBoxStyle);

    QPixmap* about_logo_pix = new QPixmap(":/src/Key Container.jpeg");
    about_logo_pix->scaled(ui->about_logoL->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->about_logoL->setScaledContents(true);
    ui->about_logoL->setPixmap(*about_logo_pix);
    QFont *about_title_font = new QFont("Microsoft YaHei");
    about_title_font->setPointSize(20);
    about_title_font->setBold(true);
    ui->about_titleL->setFont(*about_title_font);
    ui->about_titleL->setAlignment(Qt::AlignCenter);
    ui->about_plantformL->setText(ABOUT_PLANTFORM);
    ui->about_plantformL->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
    ui->about_versionL->setText(ABOUT_VERSION);
    ui->about_timeL->setText(ABOUT_TIME);
    ui->about_baseTE->clear();
    ui->about_baseTE->append(QString("C++ ") + QString::number(ABOUT_BASE_CPP));
    ui->about_baseTE->append(ABOUT_BASE_QT);
    QString ABOUT_BASE_COMPILER_type = typeid (ABOUT_BASE_COMPILER).name();
#ifdef ABOUT_BASE_COMPILER_STRING
    ui->about_baseTE->append(QString(ABOUT_BASE_COMPILER_TYPE) + QString(ABOUT_BASE_COMPILER));
#else
    ui->about_baseTE->append(QString(ABOUT_BASE_COMPILER_TYPE) + QString::number(ABOUT_BASE_COMPILER));
#endif
    ui->about_baseTE->setEnabled(false);
    ui->about_baseTE->horizontalScrollBar()->setStyle(new HorizontalScrollBarStyle);
    ui->about_baseTE->verticalScrollBar()->setStyle(new VerticalScrollBarStyle);

    fkui = new FindKeyUi(this);
    connect(this, &MainUi::mouseReleasedSig, fkui, &FindKeyUi::setTransparency, Qt::UniqueConnection);
    connect(fkui, &FindKeyUi::changeFindText, this, &MainUi::changeFindText);
    // connect上前后搜索
    connect(fkui, &FindKeyUi::findTextPrevious, this, &MainUi::findPreviousKey);
    connect(fkui, &FindKeyUi::findTextNext, this, &MainUi::findNextKey);
    // connect上计数
    connect(fkui, &FindKeyUi::countAll, this, &MainUi::countAll);
    // connect上找到结果与找不到结果的信号
    connect(this, &MainUi::sendLogLText, fkui, &FindKeyUi::setLogLText);
    connect(this, &MainUi::clearLogL, fkui, &FindKeyUi::clearLogL);
    // connect上防止搜索点击过快导致崩溃的freeze的解冻功能（冻结功能在findBtn的clicked槽函数中，自动的，不需要connect）
    connect(this, &MainUi::unfreezeFindBtn, fkui, &FindKeyUi::unfreezeFindBtn);

    // 搜索找到行
    connect(this, &MainUi::findKeyOnRow, ui->keyTW, &QTableWidget::selectRow);
    // FIXME: 具体滚动到哪一行
    connect(this, &MainUi::findKeyOnRow,  ui->keyTW->verticalScrollBar(), &QAbstractSlider::setValue);
    // 没有搜索到行

}
void MainUi::initConfig()
{
    savePath = QDir::toNativeSeparators(workPath + savePath);
    backupPath = QDir::toNativeSeparators(workPath + backupPath);
    kcdb = new Kcdb(workPath);

    if(!QFileInfo(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/config.ini")).exists()){
        log("未找到配置文件");
        return;
    }
    QSettings *ini = new QSettings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/config.ini"), QSettings::IniFormat);
    ui->savePathLE->setText(ini->value("/Path/SavePath").toString());
    kcdb->setSavePath(ui->savePathLE->text());
    ui->backupPathLE->setText(ini->value("/Path/BackupPath").toString());
    kcdb->setBackupPath(ui->backupPathLE->text());
    int selectMode = ini->value("/Setting/DefaultSelectMode").toUInt();
    switch(selectMode){
    case 0:
        ui->key_checkRB->setChecked(true);
        ui->key_check_defaultRB->setChecked(true);
        break;
    case 1:
        ui->key_clickRB->setChecked(true);
        ui->key_click_defaultRB->setChecked(true);
        connect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox, Qt::UniqueConnection);
        break;
    case 2:
        ui->key_doubleClickRB->setChecked(true);
        ui->key_doubleClick_defaultRB->setChecked(true);
        connect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox, Qt::UniqueConnection);
        break;
    default:
        ui->key_checkRB->setChecked(true);
        ui->key_check_defaultRB->setChecked(true);
    }
    autoChangeAES = ini->value("/Setting/AutoChangeAESKey").toBool();
    ui->autoChangeAESKeyChB->setChecked(autoChangeAES);
    delete ini;
    log("读取配置");
}

QWidget* MainUi::addCheckBox()
{
    QCheckBox *check = new QCheckBox(this);
//    check->installEventFilter(this);
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

void MainUi::keyTW_addNewRow(int rowIndex, Estring disc, Estring account, Estring key, int rowHeight)
{

    ui->keyTW->insertRow(rowIndex);
    ui->keyTW->setCellWidget(rowIndex, 0, addCheckBox());
    QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(rowIndex));
    idItem->setTextAlignment(Qt::AlignVCenter);
    ui->keyTW->setItem(rowIndex, 1, idItem);
    ui->keyTW->item(rowIndex,1)->setTextAlignment(Qt::AlignCenter);
    ui->keyTW->setItem(rowIndex, 2, new QTableWidgetItem(disc.getVal()));
    discQuickIndex.append(disc);
    if(is_show_pwd){
        ui->keyTW->setItem(rowIndex, 3, new QTableWidgetItem(account.getVal()));
        ui->keyTW->setItem(rowIndex, 4, new QTableWidgetItem(key.getVal()));
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
//            qDebug() << "delete" <<i;
            // 单纯的QList.removeAt并不会删除指针，用delete删除
            delete checkBoxItem[i];
            checkBoxItem.removeAt(i);
            discQuickIndex.removeAt(i);
            ui->keyTW->removeRow(i);
            tmp_deleteNum++;
            keyTableRowCount--;
        }
        else if(tmp_deleteNum!=0){
//            qDebug() << "update" <<i;
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
//            qDebug() << "pass" <<i;
            i++;
        }
    }
    keyTW_chkBoxCheckNum = 0;
    log("删除密码");
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
        log("显示密码");
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
        log("隐藏密码");
    }
}

void MainUi::addKey()
{
    KeyMap *k = new KeyMap;
    InputKeyUi *u = new InputKeyUi(this, keyTableRowCount, k);
//    u->installEventFilter(this);q
    connect(u, &InputKeyUi::inputFinish, this, [=](bool result){
        if(result){
            keyTW_addNewRow(keyTableRowCount, k->disc, k->account, k->password, 35);
            keyMap.insert(keyTableRowCount, KeyMap(keyTableRowCount, k->disc, k->account,k->password));
            keyTableRowCount++;
        }
        QApplication::instance()->installEventFilter(this);
    });
    connect(u, &InputKeyUi::inputFinish, u, &InputKeyUi::deleteLater);
    // INFO: 此处为何不需要析构？
//    connect(u, &InputKeyUi::destroyed, this, [=](){k->~KeyMap();});
    QApplication::instance()->removeEventFilter(this);
    u->show();
}

void MainUi::refreshAESKey()
{
    int max = 256;
    QString tmp = QString("12TocJn%BFde6Ng}0fGSY5s34H-PIwWEhi+#x)DuvptklabZUKq8z9jQmM$VA{R7C[X(rLOy");
    QString str;
    QTime t;
    t = QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    for(int i=0;i<max;i++)
    {
        int len = qrand()%tmp.length();
        str[i] =tmp.at(len);
    }
    kcdb->setKey_in(str);
    QString aesKeyFilePath = QDir::toNativeSeparators(workPath + "/dat.ec");
    QFile aesFile(aesKeyFilePath);
    if(aesFile.open(QIODevice::ReadWrite)){
        QDataStream aesStream(&aesFile);
        aesStream.setVersion(QDataStream::Qt_5_12);
        AesClass *de = new AesClass;
        de->initTestCase(kcdb->getKey().getVal());
        QByteArray resultAes = de->CFB256Encrypt(kcdb->getKey_in().getVal());
        aesStream << resultAes;
#ifdef DEBUG_SHOW_KEYS
        qDebug() << "refreshAESKey: write new AES key(Encrypted)=" << resultAes;
#endif
        aesFile.close();
        log("已刷新AES密钥");
    }
    else{mb.information("无法保存密码", "密码文件被其他程序占用，请重试。");}
}

bool MainUi::checkDb()
{
    QFileInfo configFileInfo(savePath+ ".chf");
//    qDebug()<<"checkDb" <<savePath+".chf";
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

            // Update(2.1.5): Encryption on *.chf
            Estring key_in;
            QFileInfo aes(kcdb->getAESKeyPath().getVal());
            if(aes.exists()){
                QFile aesHashFile(kcdb->getAESKeyPath().getVal());
                if(aesHashFile.open(QIODevice::ReadOnly)){
                    QDataStream aesStream(&aesHashFile);
                    QByteArray aesString;
                    aesStream >> aesString;
                    aesHashFile.close();
                    AesClass *de = new AesClass;
                    de->initTestCase(kcdb->getKey().getVal());
                    key_in = Estring(de->CFB256Decrypt(aesString).toUtf8());
                    delete de;
                }
            }
            else{
                mb.information("无法读取数据库密码", "密码文件可能被其他程序占用。");
                return false;
            }
            AesClass *ec = new AesClass;
            ec->initTestCase(key_in.getVal());
            QByteArray hashString_de = ec->CFB256Decrypt(hashString).toUtf8();
            delete ec;

            if(hashString_de.compare(resultHash) != 0){
                mb.information("数据库被篡改", "校验得数据库已被篡改，建议读取备份。");
                return false;
            }
            return true;
        }
        else{
            mb.information("无法校验数据库", "数据库校验文件无法打开，数据库可能能够成功读取但可能已被篡改。");
            return false;
        }
    }
    else{
        mb.information("数据库可能已被篡改", "检测不到数据库的校验文件，数据库可能能够成功读取但可能已被篡改。");
        return false;
    }

}

void MainUi::syncKeyFromMap()
{
    QMap<QString, GroupKey> tmp = kcdb->getKeys();
    keyMap.clear();
    QMap<QString, GroupKey>::const_iterator t = tmp.begin();
    qDebug() << "sync keyMap: get" << tmp.size() << "keys";
    int count=0;
    while (t != tmp.cend()) {
        // TODO: sync KeyMap id and disc -> support both version of KeyContainer
        KeyMap nKey(count, t.value().disc, t.value().account, t.value().key);
        keyMap.insert(count, nKey);
        count++;
        t++;
    }
//    qDebug() << "sync keyMap with" << count+1 << "items";
}

void MainUi::syncKeyMapToKcdb()
{
    QMap<QString, GroupKey> tmp;
    kcdb->clearKeys();
    QMap<int, KeyMap>::const_iterator t = keyMap.begin();
    while (t != keyMap.cend()) {
        GroupKey nKey(t.value().disc, t.value().account, t.value().password);

        tmp.insert(t.value().disc.getVal(), nKey);
        t++;
    }
    kcdb->setKeys(tmp);
}

void MainUi::refreshKeyTW()
{
    ui->keyTW->clearContents();
    ui->keyTW->setRowCount(0);
    QMap<int, KeyMap>::const_iterator i  = keyMap.begin();
    keyTableRowCount=0;
    while (i != keyMap.cend()) {
//        qDebug() <<"add new row to key table at" <<i->id;
        keyTW_addNewRow(keyTableRowCount, i->disc, i->account, i->password, 35);
        keyTableRowCount++;
        i++;
    }
    log("刷新列表");
}

void MainUi::writeCheckFile(QString checkPath)
{
    QFileInfo configFileInfo(checkPath);
    if(configFileInfo.exists()){
        QByteArray tmpMD5;
        QByteArray resultHash;
        QFile dbFile(checkPath);
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

        // Update(2.1.5): Encryption on *.chf
        AesClass *ec = new AesClass;
        ec->initTestCase(kcdb->getKey_in().getVal());
        QByteArray resultHash_en = ec->CFB256Encrypt(resultHash);
        delete ec;

        QString hashFilePath = checkPath + ".chf";
        QFile hashFile(hashFilePath);
        hashFile.open(QIODevice::WriteOnly);
        QDataStream outData(&hashFile);
        outData.setVersion(QDataStream::Qt_5_12);
        outData << resultHash_en;
        hashFile.close();
        log("已生成校验文件");
    }
    else{mb.information("无法生成校验文件", "无法生成校验文件，建议重新保存");}

}

void MainUi::showAcPw()
{
    delete ui->keyTW->takeItem(rightClickSelectedItemRow,3);
    delete ui->keyTW->takeItem(rightClickSelectedItemRow,4);
    if(isAcountShowing || isKeyShowing){
        ui->keyTW->setItem(rightClickSelectedItemRow, 3, new QTableWidgetItem(pwdCharacterString));
        ui->keyTW->setItem(rightClickSelectedItemRow, 4, new QTableWidgetItem(pwdCharacterString));
    }
    else{
        ui->keyTW->setItem(rightClickSelectedItemRow, 3, new QTableWidgetItem(keyMap.value(rightClickSelectedItemRow).account.getVal()));
        ui->keyTW->setItem(rightClickSelectedItemRow, 4, new QTableWidgetItem(keyMap.value(rightClickSelectedItemRow).password.getVal()));
    }
}

void MainUi::showAc()
{
    delete ui->keyTW->takeItem(rightClickSelectedItemRow,3);
    if(isAcountShowing){
        ui->keyTW->setItem(rightClickSelectedItemRow, 3, new QTableWidgetItem(pwdCharacterString));
    }
    else{
        ui->keyTW->setItem(rightClickSelectedItemRow, 3, new QTableWidgetItem(keyMap.value(rightClickSelectedItemRow).account.getVal()));
    }

}

void MainUi::showPw()
{
    delete ui->keyTW->takeItem(rightClickSelectedItemRow,4);
    if(isKeyShowing){
        ui->keyTW->setItem(rightClickSelectedItemRow, 4, new QTableWidgetItem(pwdCharacterString));
    }
    else{
        ui->keyTW->setItem(rightClickSelectedItemRow, 4, new QTableWidgetItem(keyMap.value(rightClickSelectedItemRow).password.getVal()));
    }

}

void MainUi::deleteSingleKey()
{
    //            qDebug() << "delete" <<i;
    delete checkBoxItem[rightClickSelectedItemRow];
    checkBoxItem.removeAt(rightClickSelectedItemRow);
    discQuickIndex.removeAt(rightClickSelectedItemRow);
    ui->keyTW->removeRow(rightClickSelectedItemRow);
    keyTableRowCount--;

    for(quint32 i=rightClickSelectedItemRow; i<keyTableRowCount;i++){
        // ID为行号，为保证行号与ID时刻统一，在删除时同步更新ID为新行号
        QTableWidgetItem *i1 = ui->keyTW->takeItem(i,1);
        KeyMap newKeyMapItem = keyMap[i1->text().toInt()];
        quint32 newItemId = i;
        newKeyMapItem.id = newItemId;
        keyMap.insert(newItemId, newKeyMapItem);
        keyMap.remove(i1->text().toInt());
        delete i1;
        ui->keyTW->setItem(i, 1, new QTableWidgetItem(QString::number(newItemId)));
        ui->keyTW->item(i,1)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainUi::showKeyTableMenu(QPoint)
{
    QMenu *menu = new QMenu(ui->keyTW);
    connect(menu, &QMenu::triggered, menu, &QMenu::deleteLater);
    QAction *pnew0 = new QAction("删除",ui->keyTW);
    QAction *pnew1;
    QAction *pnew2;
    QAction *pnew3;

    rightClickSelectedItemRow = (ui->keyTW->selectedItems()[0])->row();
    bool show1 = ui->keyTW->item(rightClickSelectedItemRow, 3)->text() == pwdCharacterString ? false : true;
    bool show2 = ui->keyTW->item(rightClickSelectedItemRow, 4)->text() == pwdCharacterString ? false : true;
    if(show1 || show2){
        pnew1 = new QAction("隐藏账户和密码", menu);
        if(show1){
            isAcountShowing = true;
            pnew2 = new QAction("隐藏账户", menu);
        }
        else{
            isAcountShowing = false;
            pnew2 = new QAction("显示账户", menu);
        }

        if(show2){
            isKeyShowing = true;
            pnew3 = new QAction("隐藏密码", menu);
        }
        else{
            isKeyShowing = false;
            pnew3 = new QAction("显示密码", menu);
        }

    }
    else {
        isAcountShowing = false;
        isKeyShowing = false;
        pnew1 = new QAction("显示账户和密码", menu);
        pnew2 = new QAction("显示账户", menu);
        pnew3 = new QAction("显示密码", menu);
    }

    QAction *pnew4 = new QAction("从此处向上搜索", menu);
    QAction *pnew5 = new QAction("从此处向下搜索", menu);
    connect(pnew0, &QAction::triggered, this, &MainUi::deleteSingleKey, Qt::UniqueConnection);
    connect(pnew1, &QAction::triggered, this, [&](){showAcPw();}, Qt::UniqueConnection);
    connect(pnew2, &QAction::triggered, this, [&](){showAc();}, Qt::UniqueConnection);
    connect(pnew3, &QAction::triggered, this, [&](){showPw();}, Qt::UniqueConnection);
    connect(pnew4, &QAction::triggered, this, [&](){
        this->keyTableFindPos = rightClickSelectedItemRow;
        findPreviousKey();
    }, Qt::UniqueConnection);
    connect(pnew5, &QAction::triggered, this, [&](){
        this->keyTableFindPos = rightClickSelectedItemRow;
        findNextKey();
    }, Qt::UniqueConnection);

    menu->addAction(pnew1);
    menu->addAction(pnew2);
    menu->addAction(pnew3);
    menu->addSeparator();
    menu->addAction(pnew4);
    menu->addAction(pnew5);
    menu->addSeparator();
    menu->addAction(pnew0);
    menu->move (cursor().pos());
    menu->setStyleSheet("\
        QMenu{color:rgb(240,255,255);background-color:rgb(64,66,68);border:1px solid rgb(140,155,155);}\
        QMenu::item{height:23px;}\
        QMenu::Separator{height:0px;border: 1px solid rgb(55,85,100);} \
        QMenu:selected{background-color:rgb(51,51,51);}");
    // 及时删除QMenu防止内存泄露
    connect(menu, &QMenu::aboutToHide, &QMenu::deleteLater);
    menu->show();
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
    bool a = disconnect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox);
    bool b = disconnect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox);
    if( a || b ){
        log("勾选选择");
    }
}

void MainUi::on_key_clickRB_clicked()
{
    bool a = connect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox, Qt::UniqueConnection);
    bool b = disconnect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox);
    if( a || b){
        log("单击选择");
    }
}

void MainUi::on_key_doubleClickRB_clicked()
{
    bool a = disconnect(ui->keyTW, &QTableWidget::cellClicked, this, &MainUi::selectCheckBox);
    bool b = connect(ui->keyTW, &QTableWidget::cellDoubleClicked, this, &MainUi::selectCheckBox, Qt::UniqueConnection);
    if( a || b ){
        log("双击选择");
    }
}

void MainUi::on_saveConfigBtn_clicked()
{
    QSettings *ini = new QSettings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/config.ini"), QSettings::IniFormat);
    ini->setValue("/Path/SavePath", ui->savePathLE->text().replace("\\", "/"));
    ini->setValue("/Path/BackupPath", ui->backupPathLE->text().replace("\\", "/"));
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
    log("已保存设置");
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
    InputInitKeyUi *u = new InputInitKeyUi(this, truePwdHash, Estring(workPath+"/login.ec"));
//    u->installEventFilter(this);
    connect(u, &InputInitKeyUi::changedPw, this, &MainUi::log);
    connect(u, &InputInitKeyUi::changedPw, this, [this](){
        QString pwPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() +  "/login.ec");
        QFile hashFile(pwPath);
        if(!hashFile.open(QIODevice::ReadOnly)){
            mb.information("无法读取启动密码", "密码文件可能被其他程序占用。");
            this->close();
        }
        QDataStream hashData(&hashFile);
        QByteArray hashString;
        hashData >> hashString;
        hashFile.close();
        if(hashString == ""){
            mb.information("密码错误", "检测到密码为空。");
            this->close();
        }
        truePwdHash.setVal(hashString);
        QApplication::instance()->installEventFilter(this);
    });
    QApplication::instance()->removeEventFilter(this);
    u->show();
}

void MainUi::on_saveKeyBtn_clicked()
{
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    kcdb->setBackupState(false);
    QFileInfo saveInfo(ui->savePathLE->text());
    QDir saveDir(saveInfo.path());
    if(saveDir.exists()){
        if(ui->autoChangeAESKeyChB->isChecked()){refreshAESKey();}
    }
    syncKeyMapToKcdb();
    log("正在保存数据...");
    kcdb->writeKcdb();
    writeCheckFile(savePath);
    log("数据保存完成");
}



void MainUi::on_backupKeyBtn_clicked()
{
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    kcdb->setBackupState(true);
    QFileInfo saveInfo(ui->backupPathLE->text());
    QDir saveDir(saveInfo.path());
    syncKeyMapToKcdb();
    log("正在备份数据...");
    kcdb->writeKcdb();
    writeCheckFile(backupPath);
    kcdb->setBackupState(false);
    log("数据备份完成");
}

void MainUi::on_selectSavePathBtn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择目录", workPath);
    if(path != ""){
        savePath = path + "/pw.kcdb";
        kcdb->setSavePath(savePath);
        ui->savePathLE->setText(savePath);
        ui->savePathLE->setCursorPosition(0);
    }
}

void MainUi::on_selectBackupPathBtn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择目录", workPath);
    if(path != ""){
        backupPath = path+ "/pwbp.kcdb";
        kcdb->setBackupPath(backupPath);
        ui->backupPathLE->setText(backupPath);
        ui->backupPathLE->setCursorPosition(0);
    }
}

void MainUi::on_changeAESKeyBtn_clicked()
{
    int re = mb.warning("重要提示", "重置后自动保存表格中的密码数据(覆盖旧数据)，并且会导致旧备份无法读取，是否继续？");
    if(re == mb.Yes){
        if(!ui->autoChangeAESKeyChB->isChecked()){refreshAESKey();}
        on_saveKeyBtn_clicked();
    }
    else{
        return;
    }

}

void MainUi::on_clearLogBtn_clicked()
{
    ui->logTE->clear();
}

void MainUi::on_exportKeyBtn_clicked()
{
    // yyyy-MM-dd HH:mm:ss yyyymmddHHmmss
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    QString exportPath = QFileDialog::getSaveFileName(this, "导出文件",
                             workPath + "/" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".txt", "文本文件(*.txt)");
    if(exportPath.isEmpty()){
       return;
    }
    QFile exportFile(exportPath);
    QTextStream exportStream;
    exportStream.setCodec("UTF-8");
    exportStream.setDevice(&exportFile);
    if(!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        log("导出失败，无法保存到文件");
        return;
    }
    Estring outEstring("");
    if(keyTW_chkBoxCheckNum !=0) {
        for (quint32 row=0; row< keyTableRowCount; row++) {
            if(checkBoxItem[row]->isChecked()){
                outEstring = Estring(outEstring.getVal() + "Discription=" + keyMap.value(row).disc.getVal() + \
                                     " Account=" + keyMap.value(row).account.getVal() + \
                                     " Password=" + keyMap.value(row).password.getVal() + "\n");
            }
        }
    }
    else{
        for (quint32 row=0; row< keyTableRowCount; row++) {
            outEstring = Estring(outEstring.getVal() + "Discription=" + keyMap.value(row).disc.getVal() + \
                                 " Account=" + keyMap.value(row).account.getVal() + \
                                 " Password=" + keyMap.value(row).password.getVal() + "\n");
        }
    }
    exportStream << outEstring.getVal();
    exportFile.close();
    log("导出完成： " + exportPath.replace("\\","/"));
}

void MainUi::on_backupKeysBtn_clicked()
{
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    QString newPath = QFileDialog::getExistingDirectory(this, "导出文件", workPath);
    if(newPath.isEmpty()){
        return;
    }
    QFile newDatFile(QDir::toNativeSeparators(newPath + "/dat.ec"));
    QFile oldDatFile(QDir::toNativeSeparators(workPath + "/dat.ec"));
    QFile newLoginFIle(QDir::toNativeSeparators(newPath + "/login.ec"));
    QFile oldLoginFile(QDir::toNativeSeparators(workPath + "/login.ec"));
    if(newDatFile.exists()){
        if(!newDatFile.remove()){
            log("文件已存在且无法删除: " + (QFileInfo(newDatFile)).filePath());
            return;
        }
    }
    if(!oldDatFile.copy(QFileInfo(newDatFile).filePath())){
        log("无法保存: " + (QFileInfo(newDatFile)).filePath());
        return;
    }
    if(newLoginFIle.exists()){
        if(!newLoginFIle.remove()){
            log("文件已存在且无法删除: " + (QFileInfo(newLoginFIle)).filePath());
            return;
        }
    }
    if(!oldLoginFile.copy(QFileInfo(newLoginFIle).filePath())){
        log("无法保存: " + (QFileInfo(newLoginFIle)).filePath());
        return;
    }

    log("导出完成：" + newPath.replace("\\","/"));
}


void MainUi::on_about_aboutQtB_clicked()
{
    QMessageBox::aboutQt(this, "关于Qt");
}

void MainUi::on_findKeyBtn_clicked()
{
    if(!fkui->isVisible()){

        fkui->show();
    }
    else{
        emit mouseReleasedSig(false);
        fkui->setInputFocus();
    }

}

void MainUi::findPreviousKey()
{
    if(findText == ""){
        emit sendLogLText("搜索内容为空");
        emit unfreezeFindBtn();
        return;
    }
    findDirection = false;
    emit clearLogL();
    int keyTableRowCount_int = static_cast<int>(keyTableRowCount);
    if(keyTableFindPos>=keyTableRowCount_int){
        keyTableFindPos=keyTableRowCount_int-1;
    }
    int startPos = keyTableFindPos;
    if(findMode ==0 ){
        while(keyTableFindPos>=0){
//            qDebug() <<"find previous: routine 1: at" << keyTableFindPos;
            if(!discQuickIndex[keyTableFindPos].getVal().contains(findText)){
                keyTableFindPos--;
            }
            else{
                emit findKeyOnRow(keyTableFindPos);
                emit sendLogLText("找到id: " + QString::number(keyTableFindPos));
                keyTableFindPos--;
                emit unfreezeFindBtn();
                return;
            }
        }
        // 从pos->开头找完了，接着找结尾->pos
        keyTableFindPos=keyTableRowCount_int-1;
        while(keyTableFindPos >= startPos){
//            qDebug() <<"find previous: routine 2: at" << keyTableFindPos;
            if(!discQuickIndex[keyTableFindPos].getVal().contains(findText)){
                keyTableFindPos--;
            }
            else{
                emit findKeyOnRow(keyTableFindPos);
                emit sendLogLText("找到id: " + QString::number(keyTableFindPos) + " 从最后一个开始查找");
                keyTableFindPos--;
                emit unfreezeFindBtn();
                return;
            }
        }
        keyTableFindPos = startPos;
        emit sendLogLText("搜索结果为空");
        emit unfreezeFindBtn();
        return;
    }
}

void MainUi::findNextKey()
{
//    qDebug() << "find text" << findText;
    if(findText == ""){
        emit sendLogLText("搜索内容为空");
        emit unfreezeFindBtn();
        return;
    }
    findDirection = true;
    emit clearLogL();
    int keyTableRowCount_int = static_cast<int>(keyTableRowCount);
    if(keyTableFindPos>=keyTableRowCount_int  || keyTableFindPos<0){
        keyTableFindPos=0;
    }
     int startPos = keyTableFindPos;
    if(findMode ==0 ){
        while(keyTableFindPos<keyTableRowCount_int){
//            qDebug() <<"find next: routine 1: at" << keyTableFindPos;
            if(!discQuickIndex[keyTableFindPos].getVal().contains(findText)){
                keyTableFindPos++;
            }
            else{
                emit findKeyOnRow(keyTableFindPos);
                emit sendLogLText("找到id: " + QString::number(keyTableFindPos));
                keyTableFindPos++;
                emit unfreezeFindBtn();
                return;
            }

        }
        // 从pos->结尾找完了，接着找开头->pos
        keyTableFindPos=0;
        while(keyTableFindPos <= startPos){
//            qDebug() <<"find next: routine 2: at" << keyTableFindPos;
            if(!discQuickIndex[keyTableFindPos].getVal().contains(findText)){
                keyTableFindPos++;
            }
            else{
                emit findKeyOnRow(keyTableFindPos);
                qDebug() << "from first";
                emit sendLogLText("找到id: " + QString::number(keyTableFindPos) + "  从第一个开始查找");
                keyTableFindPos++;
                emit unfreezeFindBtn();
                return;
            }
        }
        keyTableFindPos = startPos;
        emit sendLogLText("搜索结果为空");
        emit unfreezeFindBtn();
        return;
    }
}

void MainUi::changeFindMode(int mode)
{
    this->findMode = mode;
}

void MainUi::changeFindText(QString s)
{
    if(s == ""){
        return;
    }
    this->findText = s;
}

void MainUi::countAll() const
{
    int c = 0;
    foreach(Estring s, discQuickIndex){
        if(s.getVal().contains(findText)){
            c++;
        }
    }
    emit sendLogLText("计数: 共" + QString::number(c) + "个");
    emit unfreezeFindBtn();
}
