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
#include <QRegularExpression>
#include "cml/keymapjsonengine.h"
#include <QListView>
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
#include <QRandomGenerator>
#endif
//#include <QWaylandObject>

MainUi::MainUi(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainUi)
{
    appPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
#   ifndef DEBUG_SKIP_LOGIN
    logIn = new LogIn();
    connect(logIn, &LogIn::setKcdbKey, this, [=](Estring k){
        kcdb = new Kcdb(appPath + saveName, appPath + backupName);
        kcdb->setKey(k);
    });
    connect(logIn, &LogIn::finish, this, [=](bool result, Estring pwdHash){
        if(result){
            truePwdHash = pwdHash;
            loginCorrent = true;
        }
        emit open2();
    });
    connect(this, &MainUi::open2, &loginLockLoop, &QEventLoop::quit);
    if(logIn->getContinueStart()){
        logIn->setContinueStart(false);
        logIn->show();
        loginLockLoop.exec();
    }

    if(!loginCorrent){
        delete logIn;
        logIn = nullptr;
        delete kcdb;
        kcdb = nullptr;
        return;
    }
    delete logIn;
    logIn = nullptr;
    if(truePwdHash.getVal() == LOGIN_PASSWD_HASH_STR){
        MessageBoxExY::warning("需要修改密码", "检测到当前密码为默认密码，建议尽快在设置内修改", " 确定 ", "");
    }
#   else
    this->loginCorrent = true;
    kcdb = new Kcdb(appPath + saveName, appPath + backupName);
    kcdb->setKey(Estring());
#   endif
    ui->setupUi(this);
    initUi();
    initConfig();
    initKeyData();
    QApplication::instance()->installEventFilter(this);
    connect(qApp, &QGuiApplication::applicationStateChanged, this, &MainUi::appStateChanged);

}

MainUi::~MainUi()
{
    QApplication::instance()->removeEventFilter(this);
    // TODO: 为什么*ui不需要初始化，且即使初始化为nullptr在此也不是nullptr？
    // 可能ui的parent是
    if(ui==nullptr){qDebug() << "nullptr *ui";}
    delete ui;
    delete kcdb;
    delete fkui;
    if(logIn != nullptr){delete logIn; logIn = nullptr;}
    delete about_logo_pix;

    // delete styles
    delete pBStyleH;
    delete pBStyleY;
    delete hScrollBarStyle;
    delete vScrollBarStyle;
    delete rBStyle;
    delete chBStyle;
    delete cBStyle;
    delete tBStyle;
    delete tWStyle;
}

void MainUi::log(QString log)
{
    ui->logTE->append( QDateTime::currentDateTime().toString("HH:mm:ss")+ " " + log);
}

void MainUi::switchToRow(int row)
{
    ui->keyTW->selectRow(row);
    ui->keyTW->verticalScrollBar()->setValue(row);
}

void MainUi::writeInitPw(Estring p)
{
    QFile hashFile;
    if(!currentPath.isEmpty()){
        hashFile.setFileName(currentPath + LOGIN_PASSWD_FILE_NAME);
    }
    else if(kcdb->getBackupState()){
        hashFile.setFileName(QFileInfo(backupPath).path() + LOGIN_PASSWD_FILE_NAME);
    }
    else{
        hashFile.setFileName(QFileInfo(savePath).path() + LOGIN_PASSWD_FILE_NAME);
    }
    if(hashFile.open(QIODevice::WriteOnly)){
        QByteArray tmpMD5;
        QByteArray resultHash;
        QCryptographicHash hash1(QCryptographicHash::Keccak_512);
        hash1.addData(p.getVal().toUtf8());
        hash1.addData(salt1.getVal().toUtf8());
        tmpMD5 = hash1.result().toHex();
        QCryptographicHash hash2(QCryptographicHash::Keccak_512);
        hash2.addData(tmpMD5);
        hash2.addData(salt2.getVal().toUtf8());
        resultHash = hash2.result().toHex();
        QDataStream hashStream(&hashFile);
        hashStream.setVersion(QDataStream::Qt_5_11);
        hashStream << resultHash;
        hashFile.close();
//        this->close();
        emit writeInitPwSuccess();
        kcdb->setKey(p);
        if(setKcdbKey()){
            log("已更新启动密码。");
            initKeyLastWritePath = Estring(QFileInfo(hashFile).filePath());
            return;
        }
        else{
            MessageBoxExY::information("设置失败", "启动失败密码设置失败：更新Kcdb密钥失败");
            return;
        }
    }

}

//void MainUi::closeEvent(QCloseEvent *event)
//{
//    Q_UNUSED(event)
//    qDebug() << "restart:" << restart;
//    if(restart){
//        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
//    }
//}

bool MainUi::eventFilter(QObject *o, QEvent *e)
{
    if(e == nullptr){
        return false;
    }

    QMouseEvent *mouseReleased = reinterpret_cast<QMouseEvent *>(e);
    if(mouseReleased != nullptr){
//        qDebug() << "123";
        // 鼠标左键释放事件中，处理FindKeyUi透明与否
        // 自然地就是，当前tabWidget不是表格那个的时，怎么点也都是透明状态
        if(!fkui->isVisible()){
            return false;
        }
        if(mouseReleased->type() == QEvent::MouseButtonRelease){
            if(!fkui->geometry().contains(mouseReleased->globalPos()) && this->geometry().contains(mouseReleased->globalPos())){
                // 设置透明
                emit mouseReleasedSig(true);
            }
            else{
                // 设置不透明
                emit mouseReleasedSig(false);
            }

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
                    return true;
                }
            }
            else if(name.contains("TextEdit")){
                QTextEdit *e = reinterpret_cast<QTextEdit *>(o);
                if(e!=nullptr && !e->isReadOnly() && e->isEnabled()){
                    return false;
                }
                else{
                    return true;
                }
            }
            else{
                return true;
            }
        }
        return false;
    }

    return false;
}

void MainUi::keyPressEvent(QKeyEvent *e)
{
//    qDebug() << "MainUi: keyPressEvent" << e->modifiers() << e->key();
    // 键盘按键按下事件中，处理打开FindKeyUi与否
    // 用press而不用release的原因是，release的触发很慢且组合键不灵敏，，且无法按住Modifier多次触发快捷键，会显得快捷键很难用
    //  保持与FindKeyUi中的keyPressEvent相同
    if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_F){
        if(ui->mainTabWidget->currentIndex()!=0){return;}
        if(fkui->isVisible()){
            fkui->close();
            e->accept();
            return;
        }
        else{
            on_findKeyBtn_clicked();
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
                 // 判断搜索方向，执行一次搜索，先freeze再搜索，防止按快捷键多次搜索或者先unfreeze再freeze
                fkui->freezeFindBtn();
                findDirection==false ? findPreviousKey() : findNextKey();
                e->accept();
                return;
            }
        }
    }
    // Ctrl + Tab切换选项卡
    if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Tab){
        int currentIndex = ui->mainTabWidget->currentIndex();
        if(currentIndex < ui->mainTabWidget->count()-1){
            ui->mainTabWidget->setCurrentIndex(currentIndex+1);
            e->accept();
            return;
        }
        else{
            ui->mainTabWidget->setCornerWidget(0);
            e->accept();
            return;
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

    if(!checkDb(savePath)){
        log("数据校验未通过，拒绝读取数据");
        return;
    }
    ui->keyTW->setEnabled(false);
    if(kcdb->readKcdb()){
        syncKeyFromMap();
        refreshKeyTW();
    }
    ui->keyTW->setEnabled(true);
    keyTableFindPos = -1;
}

void MainUi::initUi()
{
    // styles
    pBStyleH = new PushButtonStyle("transparent");
    pBStyleY = new PushButtonStyle("transparent");
    hScrollBarStyle = new HorizontalScrollBarStyle;
    vScrollBarStyle = new VerticalScrollBarStyle;
    rBStyle = new RadioButtonStyle;
    chBStyle = new CheckBoxStyle;
    cBStyle = new ComboBoxStyle;
    tBStyle = new TabBarStyle;
    tWStyle = new TabWidgetStyle;

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/qss/stylesheet.css").arg(this->objectName()).arg("rgb(55,85,100)")
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
    ui->mainTabWidget->setTabText(1, "工具");
    ui->mainTabWidget->setTabText(2, "设置");
    ui->mainTabWidget->setTabText(3, "关于");
    ui->mainTabWidget->setTabPosition(QTabWidget::West);
    ui->mainTabWidget->setAttribute(Qt::WA_StyledBackground);
    ui->mainTabWidget->tabBar()->setStyle(tBStyle);
    ui->mainTabWidget->setStyle(tWStyle);
    ui->mainTabWidget->setCurrentIndex(0);
    ui->mainTabWidget->setFocusPolicy(Qt::NoFocus);
    // NOTE: enable page 2 in 3.2.0
//    ui->mainTabWidget->setTabEnabled(2, false);
//    ui->mainTabWidget->setTabVisible(2, false);

    QIcon tabIco0;
    const QPixmap tab0_1 = QPixmap(":/src/manage.png");
    const QPixmap tab0_2 = QPixmap(":/src/manage_reverse.png");
    if(!tab0_1.isNull() && !tab0_2.isNull()){
        tabIco0.addPixmap(tab0_1, QIcon::Selected, QIcon::Off);
        tabIco0.addPixmap(tab0_2, QIcon::Normal, QIcon::Off);
    }
    ui->mainTabWidget->tabBar()->setTabIcon(0, tabIco0);

    QIcon tabIco1;
    const QPixmap tab1_1 = QPixmap(":/src/tools.png");
    const QPixmap tab1_2 = QPixmap(":/src/tools_reverse.png");
    if(!tab1_1.isNull() && !tab1_2.isNull()){
        tabIco1.addPixmap(tab1_1, QIcon::Selected, QIcon::Off);
        tabIco1.addPixmap(tab1_2, QIcon::Normal, QIcon::Off);
    }
    ui->mainTabWidget->tabBar()->setTabIcon(1, tabIco1);

    QIcon tabIco2;
    const QPixmap tab2_1 = QPixmap(":/src/config.png");
    const QPixmap tab2_2 = QPixmap(":/src/config_reverse.png");
    if(!tab2_1.isNull() && !tab2_2.isNull()){
        tabIco2.addPixmap(tab2_1, QIcon::Selected, QIcon::Off);
        tabIco2.addPixmap(tab2_2, QIcon::Normal, QIcon::Off);

    }
    ui->mainTabWidget->tabBar()->setTabIcon(2, tabIco2);

    QIcon tabIco3;
    const QPixmap tab3_1 = QPixmap(":/src/about.png");
    const QPixmap tab3_2 = QPixmap(":/src/about_reverse.png");
    if(!tab3_1.isNull() && !tab3_2.isNull()){
        tabIco3.addPixmap(tab3_1, QIcon::Selected, QIcon::Off);
        tabIco3.addPixmap(tab3_2, QIcon::Normal, QIcon::Off);
    }
    ui->mainTabWidget->tabBar()->setTabIcon(3, tabIco3);
    ui->lockAppTimingSB->setStyle(cBStyle);
    ui->gLengthCB->setStyle(cBStyle);

    // TextEdiit
    ui->logTE->setReadOnly(true);
    ui->logTE->horizontalScrollBar()->setStyle(hScrollBarStyle);
    ui->logTE->verticalScrollBar()->setStyle(vScrollBarStyle);
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

    QIcon backupDataKeyIcon;
    const QPixmap pixm13 = QPixmap(":/src/backupKeys.png");
    if(!pixm13.isNull()){
        backupDataKeyIcon.addPixmap(pixm13, QIcon::Normal, QIcon::Off);
    }
    ui->backupDataKeyBtn->setIcon(backupDataKeyIcon);

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

    QIcon about_aboutQtIcon;
    const QPixmap pixmp15 = QPixmap(":/qt-project.org/qmessagebox/images/qtlogo-64.png");
    if(!pixmp15.isNull()){
        about_aboutQtIcon.addPixmap(pixmp15, QIcon::Normal, QIcon::Off);
    }
    ui->about_aboutQtB->setIcon(pixmp15);

    QIcon importKeysIcon;
    const QPixmap pixmp16 = QPixmap(":/src/importKeys.png");
    if(!pixmp16.isNull()){
        importKeysIcon.addPixmap(pixmp16, QIcon::Normal, QIcon::Off);
    }
    ui->importKeysBtn->setIcon(importKeysIcon);

    QIcon lockAppIcon;
    const QPixmap pixmp17 = QPixmap(":/src/lock_reverse.png");
    if(!pixmp17.isNull()){
        lockAppIcon.addPixmap(pixmp17, QIcon::Normal, QIcon::Off);
    }
    ui->lockAppBtn->setIcon(lockAppIcon);

    QIcon randomGeneratorIcon;
    const QPixmap pixmp18 = QPixmap(":/src/dice_reverse.png");
    if(!pixmp18.isNull()){
        randomGeneratorIcon.addPixmap(pixmp18, QIcon::Normal, QIcon::Off);
    }
    ui->gKeyBtn->setIcon(randomGeneratorIcon);

    QIcon copyGKeyIcon;
    const QPixmap pixmp19 = QPixmap(":/src/save_reverse.png");
    if(!pixmp19.isNull()){
        copyGKeyIcon.addPixmap(pixmp19, QIcon::Normal, QIcon::Off);
    }
    ui->gCopyResultBtn->setIcon(copyGKeyIcon);

//    PushButtonStyle *t = new PushButtonStyle(TABWIDGET_MENU_BACKGROUND_COLOR);
//    PushButtonStyle *y = new PushButtonStyle(PUSHBUTTON_ON_WIDGET_BACKGROUND_COLOR);
    // 其实这里直接用transparent就好了

    ui->addKeyBtn->setStyle(pBStyleH);
    ui->backupKeyBtn->setStyle(pBStyleH);
    ui->backupDataKeyBtn->setStyle(pBStyleH);
    ui->delSelectKeyBtn->setStyle(pBStyleY);
    ui->exportKeyBtn->setStyle(pBStyleH);
    ui->showKeyBtn->setStyle(pBStyleH);
    ui->selectAllKeyBtn->setStyle(pBStyleY);
    ui->selectInverseKeyBtn->setStyle(pBStyleY);
//    ui->clearLogBtn->setStyle(new PushButtonStyle);
    ui->saveKeyBtn->setStyle(pBStyleH);
    ui->saveConfigBtn->setStyle(pBStyleH);
    ui->restartProgBtn->setStyle(pBStyleH);
    ui->changeInitKeyBtn->setStyle(pBStyleH);
    ui->changeAESKeyBtn->setStyle(pBStyleH);
    ui->findKeyBtn->setStyle(pBStyleH);
    ui->about_aboutQtB->setStyle(pBStyleH);
    ui->importKeysBtn->setStyle(pBStyleH);
    ui->lockAppBtn->setStyle(pBStyleY);
    ui->gKeyBtn->setStyle(pBStyleY);
    ui->gCopyResultBtn->setStyle(pBStyleY);

    ui->addKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->backupKeyBtn->setFocusPolicy(Qt::NoFocus);
    ui->backupDataKeyBtn->setFocusPolicy(Qt::NoFocus);
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
    ui->about_aboutQtB->setFocusPolicy(Qt::NoFocus);
    ui->importKeysBtn->setFocusPolicy(Qt::NoFocus);
    ui->clearLogBtn->setFocusPolicy(Qt::NoFocus);

    ui->groupBox->setFocusPolicy(Qt::NoFocus);
    ui->key_checkRB->setFocusPolicy(Qt::NoFocus);
    ui->key_clickRB->setFocusPolicy(Qt::NoFocus);
    ui->key_doubleClickRB->setFocusPolicy(Qt::NoFocus);
    ui->lockAppBtn->setFocusPolicy(Qt::NoFocus);

    ui->savePathLE->setReadOnly(true);
    ui->backupPathLE->setReadOnly(true);

    ui->keyTW->setColumnCount(5);
    ui->keyTW->setHorizontalHeaderLabels({"选择", "id", "说明","账户", "密码"});
    ui->keyTW->setColumnWidth(0,40);
    ui->keyTW->setColumnWidth(1,60);
    ui->keyTW->setColumnWidth(2,150);
    ui->keyTW->setColumnWidth(3,180);
    ui->keyTW->setColumnWidth(4,200);
    ui->keyTW->horizontalHeader()->setFixedHeight(40);

    ui->keyTW->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->keyTW->setSelectionBehavior(QTableWidget::SelectRows);
    ui->keyTW->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->keyTW->verticalHeader()->setVisible(false);
    ui->keyTW->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->keyTW->horizontalHeader()->setHighlightSections(false);
    ui->keyTW->horizontalHeader()->setStretchLastSection(true);
    ui->keyTW->setAlternatingRowColors(true);
    ui->keyTW->setFocusPolicy(Qt::NoFocus);
    ui->keyTW->setFocus();
    // TODO: keyTW右键菜单
    connect(ui->keyTW, &QTableWidget::customContextMenuRequested, this, &MainUi::showKeyTableMenu, Qt::UniqueConnection);

    ui->keyTW->horizontalScrollBar()->setStyle(hScrollBarStyle);
    ui->keyTW->verticalScrollBar()->setStyle(vScrollBarStyle);
//    mb.setVerticalScrollBarStyle(vScrollBarStyle);
//    mb.setHorizontalScrollBarStyle(hScrollBarStyle);


    ui->key_checkRB->setStyle(rBStyle);
    ui->key_clickRB->setStyle(rBStyle);
    ui->key_doubleClickRB->setStyle(rBStyle);
    ui->key_check_defaultRB->setStyle(rBStyle);
    ui->key_click_defaultRB->setStyle(rBStyle);
    ui->key_doubleClick_defaultRB->setStyle(rBStyle);

    ui->autoChangeAESKeyChB->setStyle(chBStyle);
    ui->autoBackupPathChB->setStyle(chBStyle);

    about_logo_pix = new QPixmap(":/src/Key Container.jpeg");
    about_logo_pix->scaled(ui->about_logoL->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->about_logoL->setScaledContents(true);
    ui->about_logoL->setPixmap(*about_logo_pix);
//    QFont *about_title_font = new QFont("");
//    about_title_font->setPointSize(20);
//    about_title_font->setBold(true);
//    ui->about_titleL->setFont(*about_title_font);
    ui->about_titleL->setAlignment(Qt::AlignCenter);
    ui->about_plantformL->setText(ABOUT_PLANTFORM);
    ui->about_plantformL->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
    ui->about_versionL->setText(ABOUT_VERSION);
    ui->about_timeL->setText(ABOUT_TIME);
    ui->about_baseTB->clear();
    QString aboutText="";
    aboutText.append( "<p>" + QString("C++ ")+ " " + QString::number(ABOUT_BASE_CPP) + "</p>"
                      + "<p>" + ABOUT_BASE_QT+ " "  + "</p>");
    QString ABOUT_BASE_COMPILER_type = typeid (ABOUT_BASE_COMPILER).name();
#ifdef ABOUT_BASE_COMPILER_STRING
    aboutText.append("<p>" + QString(ABOUT_BASE_COMPILER_TYPE)+ " "  + QString(ABOUT_BASE_COMPILER) + "</p>");
#else
    aboutText.append("<p>" + QString(ABOUT_BASE_COMPILER_TYPE)+ " "  + QString::number(ABOUT_BASE_COMPILER) + "</p>");
#endif
    aboutText.append("<p>CapsLockCheck "
                     "<a href=\"http://github.com/savolai/Qt-Widgets-password-dialog-with-Caps-Lock-check/\"><font color=#6666FF>savolai@github</font></a></p>");
    aboutText.append("<p>Qt-AES "
                     "<a href=\"https://github.com/bricke/Qt-AES/\"><font color=#6666FF>bricke@github</font></a></p>");

    ui->about_baseTB->setReadOnly(true);
    ui->about_baseTB->setContextMenuPolicy(Qt::NoContextMenu);
    ui->about_baseTB->setOpenExternalLinks(true);
//    ui->about_baseTB->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->about_baseTB->horizontalScrollBar()->setStyle(hScrollBarStyle);
    ui->about_baseTB->verticalScrollBar()->setStyle(vScrollBarStyle);
    ui->about_baseTB->setText(aboutText);

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
    // connect改变搜索模式
    connect(fkui, &FindKeyUi::findKeyUseRegSig, this, [this](int useSig){
        if(useSig == 2){
            changeFindMode(1);
        }
        else{
            changeFindMode(0);
        }
    });

    // 搜索找到行
    connect(this, &MainUi::findKeyOnRow, this, &MainUi::switchToRow);
    // 没有搜索到行

    // 设置锁屏
    timeLocker.setSingleShot(true);
    connect(&timeLocker, &QTimer::timeout, this, &MainUi::lockApp);
    QStringList lockAppTimingSBItems{"1分钟", DEFAULT_LOCK_APP_TIME, "10分钟", "30分钟", "1小时", "3小时", "5小时"};
    timeLockerTimingMap["1分钟"]=60000;
    timeLockerTimingMap[DEFAULT_LOCK_APP_TIME]=300000;
    timeLockerTimingMap["10分钟"]=600000;
    timeLockerTimingMap["30分钟"]=1800000;
    timeLockerTimingMap["1小时"]=3600000;
    timeLockerTimingMap["3小时"]=10800000;
    timeLockerTimingMap["5小时"]=18000000;
    ui->lockAppTimingSB->addItems(lockAppTimingSBItems);
    ui->lockAppTimingSB->setView(new QListView());
    ui->lockAppTimingSB->setCurrentText(DEFAULT_LOCK_APP_TIME);
    ui->lockAppTimingSB->view()->verticalScrollBar()->setStyle(vScrollBarStyle);
//    ui->lockAppTimingSB->view()->setWindowOpacity(1);
//    ui->lockAppTimingSB->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground, false);

    ui->gResultLE->setReadOnly(true);
    ui->gResultLE->setContextMenuPolicy(Qt::NoContextMenu);
    ui->useNumberChB->setChecked(true);
    ui->useAlphaChB->setChecked(false);
    ui->useSymbolChB->setChecked(false);
    ui->useCustomCharChB->setChecked(true);
    ui->useNumberChB->setStyle(chBStyle);
    ui->useAlphaChB->setStyle(chBStyle);
    ui->useSymbolChB->setStyle(chBStyle);
    ui->useCustomCharChB->setStyle(chBStyle);
    QStringList generateLengthList={"4", "5", "6", "7", "8", "10", "16", "20", "32", "64", "128", "256"};
    ui->gLengthCB->addItems(generateLengthList);
    ui->gLengthCB->setView(new QListView());
    ui->gLengthCB->view()->verticalScrollBar()->setStyle(vScrollBarStyle);
    cboard = QApplication::clipboard();
}

void MainUi::initConfig()
{
    savePath = QDir::toNativeSeparators(appPath + saveName);
    initKeyLastWritePath.setVal(savePath);
    ui->savePathLE->setText(savePath);
    kcdb->setSavePath(savePath);
    backupPath = QDir::toNativeSeparators(appPath + backupName);
    ui->backupPathLE->setText(backupPath);
    kcdb->setBackupPath(backupPath);
    ui->key_checkRB->setChecked(true);
    ui->key_check_defaultRB->setChecked(true);
    autoChangeAES = false;
    ui->autoChangeAESKeyChB->setChecked(autoChangeAES);
    autoBackupPath = true;
    ui->autoBackupPathChB->setChecked(autoBackupPath);
    timeLockerTiming = timeLockerTimingMap[DEFAULT_LOCK_APP_TIME];
    ui->lockAppTimingSB->setCurrentText(DEFAULT_LOCK_APP_TIME);
    generateLength = DEFAULT_STR_GEN_LENGTH;
    ui->gLengthCB->setCurrentText(QString::number(generateLength));

    if(!QFileInfo(QDir::toNativeSeparators(appPath + "/config.ini")).exists()){
        log("未找到配置文件");
        return;
    }
    QSettings *ini = new QSettings(QDir::toNativeSeparators(appPath + "/config.ini"), QSettings::IniFormat);
    QFileInfo saveFile(ini->value("/Path/SavePath").toString());
    if(saveFile.exists() && saveFile.isFile()){
        savePath = saveFile.absoluteFilePath();
        saveName = "/" + saveFile.fileName();
        initKeyLastWritePath.setVal(savePath);
        ui->savePathLE->setText(savePath);
        kcdb->setSavePath(savePath);
    }

    QFileInfo backupFile(ini->value("/Path/BackupPath").toString());
    if(backupFile.exists() && backupFile.isFile()){
        backupPath = backupFile.absoluteFilePath();
        backupName = "/" + backupFile.fileName();
        ui->backupPathLE->setText(backupPath);
        kcdb->setBackupPath(backupPath);
    }
    int selectMode = ini->value("/Common/DefaultSelectMode").toUInt();
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
    autoChangeAES = ini->value("/Security/AutoChangeAESKey").toBool();
    ui->autoChangeAESKeyChB->setChecked(autoChangeAES);
    autoBackupPath = ini->value("/Common/AutoBackupPath").toBool();
    ui->autoBackupPathChB->setChecked(autoBackupPath);
    QString lockAppTimingTmp = ini->value("/Security/LockAppTiming").toString().replace("m", "分钟").replace("h", "小时");
    if(timeLockerTimingMap.contains(lockAppTimingTmp)){
        timeLockerTiming = timeLockerTimingMap[lockAppTimingTmp];
        ui->lockAppTimingSB->setCurrentText(lockAppTimingTmp);
    }
    generateLength = qMax(ini->value("/Tool/GeneratorGenerateLength").toInt(), GENERATOR_MIN_LENGTH);
    ui->gLengthCB->setCurrentText(QString::number(generateLength));

    delete ini;
    log("读取配置");
}

QWidget* MainUi::addCheckBox(int height)
{
    Q_UNUSED(height)
    QCheckBox *check = new QCheckBox(ui->keyTW);
//    check->setFixedSize(height, height);

//    check->installEventFilter(this);
    check->setStyle(chBStyle);
    checkBoxItem.append(check);
    QHBoxLayout *hb = new QHBoxLayout(ui->keyTW);
    hb->addWidget(check);
//    hb->setMargin(0);
    hb->setAlignment(Qt::AlignHCenter);
    // 设置居中的关键 setContentsMargins，适应win10各缩放比例
    hb->setContentsMargins(10, 0, 10, 0);

//    connect(check, SIGNAL(stateChanged(int)), this, SLOT(selectChecked(int)));
    connect(check, &QCheckBox::stateChanged, this, &MainUi::selectChecked);
    QWidget *resultQWidget = new QWidget();
    resultQWidget->setLayout(hb);
    return resultQWidget;
}

void MainUi::keyTW_addNewRow(int rowIndex, Estring disc, Estring account, Estring key, int rowHeight, bool show)
{
    ui->keyTW->insertRow(rowIndex);
    ui->keyTW->setCellWidget(rowIndex, 0, addCheckBox(rowHeight));
    QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(rowIndex));
    idItem->setTextAlignment(Qt::AlignVCenter);
    ui->keyTW->setItem(rowIndex, 1, idItem);
    ui->keyTW->item(rowIndex,1)->setTextAlignment(Qt::AlignCenter);
    ui->keyTW->setItem(rowIndex, 2, new QTableWidgetItem(disc.getVal()));
    discQuickIndex.append(disc);
    if(is_show_pwd && (show || static_cast<quint32>(keysShowsNumber)==2*keyTableRowCount)){
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
            // 单纯的QList.removeAt并不会删除指针，用delete删除
            keyMap.remove(ui->keyTW->item(i, 1)->text().toInt());
            delete checkBoxItem[i];
            checkBoxItem.removeAt(i);
            discQuickIndex.removeAt(i);
            ui->keyTW->removeRow(i);
            tmp_deleteNum++;
            keyTableRowCount--;
        }
        else if(tmp_deleteNum!=0){
            // ID为行号，为保证行号与ID时刻统一，在删除时同步更新ID为新行号
            QTableWidgetItem *i1 = ui->keyTW->takeItem(i,1);
            KeyMap newKeyMapItem = keyMap[i1->text().toInt()];
            quint32 newItemId = i;
            newKeyMapItem.id = newItemId;
            keyMap.insert(newItemId, newKeyMapItem);
            keyMap.remove(i1->text().toInt());
            delete i1;
            QTableWidgetItem *i2 = new QTableWidgetItem(QString::number(newItemId));
            i2->setTextAlignment(Qt::AlignCenter);
            ui->keyTW->setItem(i, 1, i2);
            i++;
        }
        else{
            i++;
        }
    }
    keyTW_chkBoxCheckNum = 0;
    qDebug() << keyMap.count();
    log("删除密码");
}

void MainUi::showKeyTableKeys()
{
    if(!is_show_pwd){
        for(quint32 i=0; i<keyTableRowCount; i++){
            // TODO: 此处应该显示的账号及密码
            // 注意delete 与 takeItem是否有问题
            QTableWidgetItem* i1 = ui->keyTW->item(i,1);
            ui->keyTW->item(i,3)->setText(keyMap.value(i1->text().toInt()).account.getVal());
            ui->keyTW->item(i,4)->setText(keyMap.value(i1->text().toInt()).password.getVal());
//            ui->keyTW->setItem(i, 3, new QTableWidgetItem(decrypt(keyMap.value(i1->text().toInt()).account)));
        }
        keysShowsNumber = keyTableRowCount*2;
        is_show_pwd = true;
        ui->showKeyBtn->setText("隐藏密码");
        log("显示密码");
    }
    else{
        for(quint32 i=0; i<keyTableRowCount; i++){
            ui->keyTW->item(i,3)->setText(pwdCharacterString);
            ui->keyTW->item(i,4)->setText(pwdCharacterString);
        }
        keysShowsNumber =0;
        is_show_pwd = false;
        ui->showKeyBtn->setText("显示密码");
        log("隐藏密码");
    }
}

void MainUi::addKey()
{
    KeyMap *k = new KeyMap;
    InputKeyUi *u = new InputKeyUi(this, keyTableRowCount, k, discQuickIndex);
//    u->installEventFilter(this);q
    connect(u, &InputKeyUi::finished, this, [this](){
        QApplication::instance()->installEventFilter(this);
    });
    connect(u, &InputKeyUi::inputFinish, this, [=](bool result, int existPos){
        // 需要加入密码
        if(result){
            // 密码已经存在，更新旧密码
            // TODO: 这里或许应该去掉的，qml版暂时没有在这里加更新旧密码的功能
            // qml版的更改密码是另一个独立功能，widget版应该也加上
            if(existPos>=0){
                keyMap[existPos].disc = k->disc;
                keyMap[existPos].account = k->account;
                keyMap[existPos].password = k->password;
                discQuickIndex[existPos] = k->disc;
                QTableWidgetItem *i1 = ui->keyTW->item(existPos, 3);
                if(i1->text() != pwdCharacterString){
                    i1->setText(k->account.getVal());
                }
                QTableWidgetItem *i2 = ui->keyTW->item(existPos, 4);
                if(i2->text() != pwdCharacterString){
                    i2->setText(k->password.getVal());
                }
            }
            // 密码不存在，加入新密码
            // TODO: 新密码固定加入最后一行，或许该考虑直接加到准确位置
            else{
                keyTW_addNewRow(keyTableRowCount, k->disc, k->account, k->password, 35, false);
                keyMap.insert(keyTableRowCount, KeyMap(keyTableRowCount, k->disc, k->account,k->password));
                keyTableRowCount++;
            }
        }
    });
    connect(u, &InputKeyUi::inputFinish, u, &InputKeyUi::deleteLater);
    // INFO: 此处为何不需要析构？
//    connect(u, &InputKeyUi::destroyed, this, [=](){k->~KeyMap();});
    QApplication::instance()->removeEventFilter(this);
    u->exec();
    delete u;
}

void MainUi::refreshAESKey()
{
    int max = 256;
    QString tmp = QString("12TocJn%BFde6Ng}0fGSY5s34H-PIwWEhi+#x)DuvptklabZUKq8z9jQmM$VA{R7C[X(rLOy");
    QString str;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
    for(int i=0;i<max;i++)
    {
        int len = QRandomGenerator::securelySeeded().bounded(0,tmp.length());
        str[i] =tmp.at(len);
    }
#else
    QTime t;
    t = QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    for(int i=0;i<max;i++)
    {
        int len = qrand()%tmp.length();
        str[i] =tmp.at(len);
    }
#endif
    kcdb->setKey_in(Estring(str));
    setKcdbKey() ? log("已刷新AES密钥") : MessageBoxExY::information("无法保存密码", "密码文件被其他程序占用，请重试。");
}

bool MainUi::checkDb(QString dbPath)
{
    QFileInfo configFileInfo;
    QFile dbFile;
    QString hashFilePath;
    configFileInfo.setFile(dbPath+ ".chf");
    dbFile.setFileName(dbPath);
    hashFilePath = dbPath + ".chf";

    if(configFileInfo.exists()){
        QByteArray tmpMD5;
        QByteArray resultHash;
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
        QFile hashFile(hashFilePath);
        if(hashFile.open(QIODevice::ReadOnly)){
            QDataStream hashData(&hashFile);
            hashData.setVersion(QDataStream::Qt_5_11);
            QByteArray hashString;
            hashData >> hashString;
            hashFile.close();

            // Update(2.1.5): Encryption on *.chf
            Estring key_in;
            QString aesFilePath = QDir::toNativeSeparators(configFileInfo.path().replace("\\", "/") + KEYDB_PASSWD_FILE_NAME);

            QFileInfo aes(aesFilePath);
#ifdef DEBUG_SHOW_IO_PATH
            qDebug() << "checkDb:" << "read " << KEYDB_PASSWD_FILE_NAME << " hash data from " << hashFile.fileName();
            qDebug() << "checkDb:" << "read " << KEYDB_PASSWD_FILE_NAME << " data from " << aes.filePath();
#endif
            if(aes.exists()){
                QFile aesHashFile(aesFilePath);
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
                MessageBoxExY::information("无法读取数据库密码", "密码文件可能被其他程序占用。");
                return false;
            }
            AesClass *ec = new AesClass;
            ec->initTestCase(key_in.getVal());
            QByteArray hashString_de = ec->CFB256Decrypt(hashString).toUtf8();
            delete ec;
#if QT_VERSION >= QT_VERSION_CHECK(5,12,0)
            if(hashString_de.compare(resultHash) != 0){
#else
            if(hashString_de == resultHash){
#endif
                MessageBoxExY::information("数据库被篡改", QString("校验得数据库已被篡改，建议读取备份。\n"
                                               "file:%1\n"
                                               "check_file:%2").arg(dbPath).arg(hashFilePath));
                return false;
            }
            return true;
        }
        else{
            MessageBoxExY::information("无法校验数据库", "数据库校验文件无法打开。");
            return false;
        }
    }
    else{
        MessageBoxExY::information("数据库可能已被篡改", "检测不到数据库的校验文件。");
        return false;
    }

}

void MainUi::syncKeyFromMap()
{
    QMap<QString, GroupKey> tmp = kcdb->getKeys();
    keyMap.clear();
    QMap<QString, GroupKey>::const_iterator t = tmp.begin();
    int count=0;
    while (t != tmp.cend()) {
        // TODO: sync KeyMap id and disc -> support both version of KeyContainer
        KeyMap nKey(count, t.value().disc, t.value().account, t.value().key);
        keyMap.insert(count, nKey);
        count++;
        t++;
    }
}

void MainUi::syncKeyMapToKcdb()
{
    QMap<QString, GroupKey> tmp;
    kcdb->clearKeys();
    QMap<int, KeyMap>::const_iterator t = keyMap.cbegin();
    while (t != keyMap.cend()) {
        GroupKey nKey(t.value().disc, t.value().account, t.value().password);

        tmp.insert(t.value().disc.getVal(), nKey);
        t++;
    }
    kcdb->setKeys(tmp);
    qDebug() << "sync to kcdb" << kcdb->getKeys().count();
}

void MainUi::refreshKeyTW()
{
    checkBoxItem.clear();
    discQuickIndex.clear();
    ui->keyTW->clearContents();
    ui->keyTW->setRowCount(0);
    QMap<int, KeyMap>::const_iterator i  = keyMap.cbegin();
    keyTableRowCount=0;
    while (i != keyMap.cend()) {
        keyTW_addNewRow(keyTableRowCount, i->disc, i->account, i->password, 35);
        keyTableRowCount++;
        i++;
    }
    log("刷新列表");
}

void MainUi::writeCheckFile(QString checkPath)
{
    QFileInfo configFileInfo(checkPath);
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
#ifdef DEBUG_SHOW_IO_PATH
    qDebug() << "writeCheckFile:" << "write check data to " << hashFilePath;
#endif
    QFile hashFile(hashFilePath);
    hashFile.open(QIODevice::WriteOnly);
    QDataStream outData(&hashFile);
    outData.setVersion(QDataStream::Qt_5_11);
    outData << resultHash_en;
    hashFile.close();

    if(configFileInfo.exists() && configFileInfo.isFile()){
        log("已生成校验文件");
    }
    else{
        MessageBoxExY::information("无法生成校验文件", "无法生成校验文件，建议重新保存");
    }
}

void MainUi::showAcPw()
{
    if(isAcountShowing || isKeyShowing){
        if(isAcountShowing){
            showAc();
        }
        if(isKeyShowing){
            showPw();
        }
    }
    else{
        showAc();
        showPw();
    }

    if(keysShowsNumber<=0){
        is_show_pwd = false;
        ui->showKeyBtn->setText("显示密码");
    }
    else{
        is_show_pwd = true;
        ui->showKeyBtn->setText("隐藏密码");
    }
}

void MainUi::showAc()
{
    if(isAcountShowing){
        ui->keyTW->item(rightClickSelectedItemRow, 3)->setText(pwdCharacterString);
        keysShowsNumber--;
        if(keysShowsNumber<=0){
            is_show_pwd = false;
            ui->showKeyBtn->setText("显示密码");
        }
    }
    else{
        ui->keyTW->item(rightClickSelectedItemRow, 3)->setText(keyMap.value(rightClickSelectedItemRow).account.getVal());
        keysShowsNumber++;
        is_show_pwd = true;
        ui->showKeyBtn->setText("隐藏密码");
    }

}

void MainUi::showPw()
{
    if(isKeyShowing){
        ui->keyTW->item(rightClickSelectedItemRow, 4)->setText(pwdCharacterString);
        keysShowsNumber--;
        if(keysShowsNumber<=0){
            is_show_pwd = false;
            ui->showKeyBtn->setText("显示密码");
        }
    }
    else{
        ui->keyTW->item(rightClickSelectedItemRow, 4)->setText(keyMap.value(rightClickSelectedItemRow).password.getVal());
        keysShowsNumber++;
        is_show_pwd = true;
        ui->showKeyBtn->setText("隐藏密码");
    }

}

void MainUi::deleteSingleKey()
{
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

bool MainUi::setKcdbKey(QString keyPath)
{
    QFile aesFile;
    if(keyPath.isEmpty()){
        if(kcdb->getBackupState()){
            aesFile.setFileName(QFileInfo(backupPath).path() + KEYDB_PASSWD_FILE_NAME);
        }
        else{
            aesFile.setFileName(QFileInfo(savePath).path() + KEYDB_PASSWD_FILE_NAME);
        }
    }
    else{
        aesFile.setFileName(QFileInfo(keyPath).path() + KEYDB_PASSWD_FILE_NAME);
    }

//    QString aesKeyFilePath = QDir::toNativeSeparators(kcdb->getSaveAESKeyPath().getVal());
//    QFile aesFile(aesKeyFilePath);
    if(aesFile.open(QIODevice::ReadWrite)){
        QDataStream aesStream(&aesFile);
        aesStream.setVersion(QDataStream::Qt_5_11);
        AesClass *de = new AesClass;
        de->initTestCase(kcdb->getKey().getVal());
        QByteArray resultAes = de->CFB256Encrypt(kcdb->getKey_in().getVal());
        aesStream << resultAes;
#ifdef DEBUG_SHOW_KEYS
        qDebug() << "refreshAESKey: write new AES key(Encrypted)=" << resultAes;
#endif
        aesFile.close();
        delete de;
        return true;
    }
    return false;
}

Estring MainUi::randomGenerator()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    if(!(useNumbers || useAlphas || useSymbols || (useCustom && !ui->gCustomCharLE->text().isEmpty()))){
        log("Empty input");
        return Estring();
    }
    QString numbers="0123456789";
    QString alphas="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString symbols="~!@#$%^&*()_+`-={}|[]:,./<>?;'\\\"";
    QString seedString="";
    QString resultString="";
    if(useNumbers){
        seedString+=numbers;
    }
    if(useAlphas){
        seedString+=alphas;
    }
    if(useSymbols){
        seedString+=symbols;
    }
    if(useCustom){
        seedString+=ui->gCustomCharLE->text();
    }
    generateLength=qMax(generateLength, GENERATOR_MIN_LENGTH);
    resultString += seedString[QRandomGenerator::securelySeeded().bounded(0, seedString.length())];
    while (resultString.length() < generateLength) {
        resultString.insert(QRandomGenerator::securelySeeded().bounded(0, resultString.length()),
                            seedString[QRandomGenerator::securelySeeded().bounded(0, seedString.length())]);
    }
    return Estring(resultString);
#else
    log("randomGenerator not work when Qt version < 5.10.0");
    return Estring();
#endif
}

void MainUi::showKeyTableMenu(QPoint pos)
{
    if(ui->keyTW->itemAt(pos) == nullptr){
        return;
    }

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
    menu->addAction(pnew1);
    menu->addAction(pnew2);
    menu->addAction(pnew3);

    connect(pnew0, &QAction::triggered, this, &MainUi::deleteSingleKey, Qt::UniqueConnection);
    connect(pnew1, &QAction::triggered, this, [&](){showAcPw();}, Qt::UniqueConnection);
    connect(pnew2, &QAction::triggered, this, [&](){showAc();}, Qt::UniqueConnection);
     connect(pnew3, &QAction::triggered, this, [&](){showPw();}, Qt::UniqueConnection);
     if(this->enableKeyTWContextMenuSearch){
         QAction *pnew4 = new QAction("从此处向上搜索", menu);
         QAction *pnew5 = new QAction("从此处向下搜索", menu);
         connect(pnew4, &QAction::triggered, this, [&](){
             this->keyTableFindPos = rightClickSelectedItemRow;
             findPreviousKey();
         }, Qt::UniqueConnection);
         connect(pnew5, &QAction::triggered, this, [&](){
             this->keyTableFindPos = rightClickSelectedItemRow;
             findNextKey();
         }, Qt::UniqueConnection);
         menu->addSeparator();
         menu->addAction(pnew4);
         menu->addAction(pnew5);
     }
    menu->addSeparator();
    menu->addAction(pnew0);
    menu->move (cursor().pos());
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
    QSettings *ini = new QSettings(QDir::toNativeSeparators(appPath + "/config.ini"), QSettings::IniFormat);
    ini->setValue("/Path/SavePath", ui->savePathLE->text().replace("\\", "/"));
    ini->setValue("/Path/BackupPath", ui->backupPathLE->text().replace("\\", "/"));
    // 同时还要同步两个变量
    kcdb->setSavePath(ui->savePathLE->text());
    kcdb->setBackupPath(ui->backupPathLE->text());
    if(ui->key_check_defaultRB->isChecked()){
        ini->setValue("/Common/DefaultSelectMode", 0);
    }
    else if(ui->key_click_defaultRB->isChecked()){
        ini->setValue("/Common/DefaultSelectMode", 1);
    }
    else{
        ini->setValue("/Common/DefaultSelectMode", 2);
    }
    ini->setValue("/Security/AutoChangeAESKey", autoChangeAES);
    ini->setValue("/Common/AutoBackupPath", ui->autoBackupPathChB->isChecked());
    ini->setValue("/Security/LockAppTiming", ui->lockAppTimingSB->currentText().replace("分钟", "m").replace("小时", "h"));
    ini->setValue("/Tool/GeneratorGenerateLength", generateLength);
    delete ini;
    log("已保存设置");
}

void MainUi::on_autoChangeAESKeyChB_stateChanged(int arg1)
{
    arg1==0 ? autoChangeAES=false : autoChangeAES=true;
}

void MainUi::on_restartProgBtn_clicked()
{

    restart=true;
//    this->close();
    QApplication::closeAllWindows();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void MainUi::on_changeInitKeyBtn_clicked()
{
    InputInitKeyUi *u = new InputInitKeyUi(this, truePwdHash, Estring(currentPath.isEmpty() ? appPath + LOGIN_PASSWD_FILE_NAME : currentPath + LOGIN_PASSWD_FILE_NAME));
//    u->installEventFilter(this);
    connect(u, &InputInitKeyUi::finished, this, [this](){
        QApplication::instance()->installEventFilter(this);
        qDebug() << "installed event filter";
    });
    connect(this, &MainUi::writeInitPwSuccess, u, &InputInitKeyUi::close, Qt::UniqueConnection);
    connect(this, &MainUi::writeInitPwSuccess, this, [this](){
            QString pwPath = QDir::toNativeSeparators(currentPath.isEmpty() ? appPath + LOGIN_PASSWD_FILE_NAME : currentPath + LOGIN_PASSWD_FILE_NAME);
            QFile hashFile(pwPath);
            if(!hashFile.open(QIODevice::ReadOnly)){
                MessageBoxExY::information("无法读取启动密码", "密码文件可能被其他程序占用。");
                this->close();
            }
            QDataStream hashData(&hashFile);
            QByteArray hashString;
            hashData >> hashString;
            hashFile.close();
            if(hashString == ""){
                MessageBoxExY::information("密码错误", "检测到密码为空。");
                this->close();
            }
            truePwdHash.setVal(hashString);
        }, Qt::UniqueConnection);
    connect(u, &InputInitKeyUi::writePw, this, &MainUi::writeInitPw, Qt::UniqueConnection);
    QApplication::instance()->removeEventFilter(this);
    u->exec();
    delete u;
}

void MainUi::on_saveKeyBtn_clicked()
{
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    if((!currentPath.isEmpty()) && (currentPath != savePath)){
        int result = MessageBoxExY::question("保存路径非默认", "当前导入数据库不是设置中的数据库，是否继续保存到当前位置？\n当前位置: " + currentPath);
        if(result == MessageBoxExX::Yes){
            kcdb->setBackupState(false);
            QFileInfo saveInfo(currentPath);
            QDir saveDir(saveInfo.path());
            if(saveDir.exists()){
                if(ui->autoChangeAESKeyChB->isChecked()){refreshAESKey();}
            }
            syncKeyMapToKcdb();
            log("正在保存数据...");
            QFile::copy(QFileInfo(initKeyLastWritePath.getVal()).path() + LOGIN_PASSWD_FILE_NAME, QFileInfo(savePath).path() + LOGIN_PASSWD_FILE_NAME);
            setKcdbKey(currentPath);
            if(kcdb->writeKcdb(currentPath)){
                writeCheckFile(currentPath);
                log("数据保存完成");
//                bool autoBackupPathOld=autoBackupPath;
//                autoBackupPath=true;
//                on_backupKeyBtn_clicked();
//                autoBackupPath=autoBackupPathOld;
            }
            else{
                log("保存失败");
            }
        }
        else{
            return;
        }
    }
    else{
        kcdb->setBackupState(false);
        QFileInfo saveInfo(savePath);
        QDir saveDir(saveInfo.path());
        if(saveDir.exists()){
            if(ui->autoChangeAESKeyChB->isChecked()){refreshAESKey();}
        }
        syncKeyMapToKcdb();
        log("正在保存数据...");
        QFile::copy(QFileInfo(initKeyLastWritePath.getVal()).path() + LOGIN_PASSWD_FILE_NAME, QFileInfo(savePath).path() + LOGIN_PASSWD_FILE_NAME);
        setKcdbKey();
        if(kcdb->writeKcdb(savePath)){
            writeCheckFile(savePath);
            log("数据保存完成");
            bool autoBackupPathOld=autoBackupPath;
            autoBackupPath=true;
            on_backupKeyBtn_clicked();
            autoBackupPath=autoBackupPathOld;
        }
        else{
            log("保存失败");
        }
    }
}

void MainUi::on_backupKeyBtn_clicked()
{
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    QString finalPath;
    // 是否跳过选择备份目录
    if(!autoBackupPath){
        int result = MessageBoxExY::question("备份密码", "可以选择其他的位置保存数据，是否要选其他位置保存？", "换个位置", "不换了");
        if(result == MessageBoxExX::Yes){
            QString newPath = QFileDialog::getExistingDirectory(this, "选择路径", appPath, QFileDialog::ShowDirsOnly);
            if(newPath.isEmpty()){
                return;
            }
            finalPath = QDir::toNativeSeparators(newPath + backupName);
        }
        else if(result == MessageBoxExX::No){
            finalPath = backupPath;
        }
        else{
            return;
        }
    }
    else{
        finalPath = backupPath;
    }
    kcdb->setBackupState(true);
    QFileInfo saveInfo(finalPath);
    QDir saveDir(saveInfo.path());
    syncKeyMapToKcdb();
    log("正在备份数据...");
    QFile::copy(QFileInfo(initKeyLastWritePath.getVal()).path() + LOGIN_PASSWD_FILE_NAME, QFileInfo(finalPath).path() + LOGIN_PASSWD_FILE_NAME);
    // NOTE: 这里是手动一次性备份到其他目录，不止需要kcdb->setBackupState(true)，还需要暂时设置backupPath路径，备份完成后改回去
//    QString oldBackupPath = backupPath;
//    backupPath = finalPath;
    setKcdbKey(finalPath);
//    backupPath = oldBackupPath;
    if(kcdb->writeKcdb(finalPath)){
        writeCheckFile(finalPath);
        log("数据备份完成");
    }
    else{
        log("备份失败");
    }

}

void MainUi::on_selectSavePathBtn_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "保存到", savePath + saveName, PASSWD_FILE_FORM_KCDB);
    if(path != ""){
        savePath = path;
        saveName = QFileInfo(path).fileName();
        kcdb->setSavePath(savePath);
        ui->savePathLE->setText(savePath);
        ui->savePathLE->setCursorPosition(0);
    }
}

void MainUi::on_selectBackupPathBtn_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "备份到", backupPath + backupName, PASSWD_FILE_FORM_KCDB);
    if(path != ""){
        backupPath = path;
        backupName = QFileInfo(path).fileName();
        kcdb->setBackupPath(backupPath);
        ui->backupPathLE->setText(backupPath);
        ui->backupPathLE->setCursorPosition(0);
    }
}

void MainUi::on_changeAESKeyBtn_clicked()
{
    int re = MessageBoxExY::warning("重要提示", "重置后自动保存表格中的密码数据(覆盖旧数据)，并且会导致旧备份无法读取，是否继续？");
    if(re == MessageBoxExX::Yes){
        if(ui->autoChangeAESKeyChB->isChecked()){refreshAESKey();}
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
                             appPath + "/" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".kcdj", PASSWD_FILE_FORM_JSON);
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
    // 2.3.0.0版本及以后的导出密码，导出json明文
    QList<bool> select;
    int exportCount=0;
    foreach(QCheckBox *c, checkBoxItem){
        select.append(c->isChecked());
        if(!c->isChecked()){
            exportCount++;
        }
    }
    if(exportCount==checkBoxItem.count()){
        outEstring.setVal(KeyMapJsonEngine::keyMapToJson(keyMap));
    }
    else{
        outEstring.setVal(KeyMapJsonEngine::keyMapToJson(keyMap, select));
    }
    // 2.2.16.8版本及以前的导出密码，导出明文
    /*
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
   */
    exportStream << outEstring.getVal();
    exportFile.close();
    log("导出完成： " + exportPath.replace("\\","/"));
}

void MainUi::on_backupDataKeyBtn_clicked()
{
    if(keyTableRowCount==0){
        log("密码为空");
        return;
    }
    QString newPath = QFileDialog::getExistingDirectory(this, "导出文件", savePath);
    if(newPath.isEmpty()){
        return;
    }
    if(newPath == savePath || newPath == backupPath){
        log("无效路径");
        return;
    }
    QFile newDatFile(QDir::toNativeSeparators(newPath + KEYDB_PASSWD_FILE_NAME));
    QFile oldDatFile(QDir::toNativeSeparators(savePath + KEYDB_PASSWD_FILE_NAME));
    QFile newLoginFIle(QDir::toNativeSeparators(newPath + LOGIN_PASSWD_FILE_NAME));
    QFile oldLoginFile(QDir::toNativeSeparators(savePath + LOGIN_PASSWD_FILE_NAME));
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

bool MainUi::findCheckKey()
{
    if(findMode == 0 ){
        return fkui->isFindAllWord() ?
                    !(discQuickIndex[keyTableFindPos].getVal().compare(findText, fkui->isCaseSen()) == 0) :
                    !discQuickIndex[keyTableFindPos].getVal().contains(findText, fkui->isCaseSen());
    }
    else if(findMode == 1){
        return !discQuickIndex[keyTableFindPos].getVal().contains(QRegularExpression(findText));
    }
    else{
        return true;
    }
}

void MainUi::findPreviousKey()
{
    if(keyTableRowCount <= 0){
        emit sendLogLText("列表为空");
        emit unfreezeFindBtn();
        return;
    }
    if(findText == ""){
        emit sendLogLText("搜索内容为空");
        emit unfreezeFindBtn();
        return;
    }
    findDirection = false;
    emit clearLogL();
    int keyTableRowCount_int = static_cast<int>(keyTableRowCount);
    keyTableFindPos--;
    if(keyTableFindPos>=keyTableRowCount_int || keyTableFindPos<0){
        keyTableFindPos=keyTableRowCount_int-1;
    }
    int startPos = keyTableFindPos;
    while(keyTableFindPos>=0){
        if(findCheckKey()){
            keyTableFindPos--;
        }
        else{
            emit findKeyOnRow(keyTableFindPos);
            emit sendLogLText("找到id: " + QString::number(keyTableFindPos));
            emit unfreezeFindBtn();
            return;
        }
    }
    // 从pos->开头找完了，接着找结尾->pos
    keyTableFindPos=keyTableRowCount_int-1;
    while(keyTableFindPos >= startPos){
        if(findCheckKey()){
            keyTableFindPos--;
        }
        else{
            emit findKeyOnRow(keyTableFindPos);
            emit sendLogLText("找到id: " + QString::number(keyTableFindPos) + " 从最后一个开始查找");
            emit unfreezeFindBtn();
            return;
        }
    }
    keyTableFindPos = startPos;
    emit sendLogLText("搜索结果为空");
    emit unfreezeFindBtn();
    return;
}

void MainUi::findNextKey()
{
    if(keyTableRowCount <= 0){
        emit sendLogLText("列表为空");
        emit unfreezeFindBtn();
        return;
    }
    if(findText == ""){
        emit sendLogLText("搜索内容为空");
        emit unfreezeFindBtn();
        return;
    }
    findDirection = true;
    emit clearLogL();
    int keyTableRowCount_int = static_cast<int>(keyTableRowCount);
    keyTableFindPos++;
    if(keyTableFindPos>=keyTableRowCount_int  || keyTableFindPos<0){
        keyTableFindPos=0;
    }
     int startPos = keyTableFindPos;
    while(keyTableFindPos<keyTableRowCount_int){
        if(findCheckKey()){
            keyTableFindPos++;
        }
        else{
            emit findKeyOnRow(keyTableFindPos);
            emit sendLogLText("找到id: " + QString::number(keyTableFindPos));
            emit unfreezeFindBtn();
            return;
        }
    }
    // 从pos->结尾找完了，接着找开头->pos
    keyTableFindPos=0;
    while(keyTableFindPos <= startPos){
        if(findCheckKey()){
            keyTableFindPos++;
        }
        else{
            emit findKeyOnRow(keyTableFindPos);
            emit sendLogLText("找到id: " + QString::number(keyTableFindPos) + "  从第一个开始查找");
            emit unfreezeFindBtn();
            return;
        }
    }
    keyTableFindPos = startPos;
    emit sendLogLText("搜索结果为空");
    emit unfreezeFindBtn();
    return;
}

void MainUi::changeFindMode(int mode)
{
    this->findMode = mode;
}

void MainUi::changeFindText(QString s)
{
    if(s == ""){
        this->enableKeyTWContextMenuSearch = false;
        return;
    }
    this->enableKeyTWContextMenuSearch = true;
    this->findText = s;
}

void MainUi::countAll()
{
    if(keyTableRowCount <= 0){
        emit sendLogLText("列表为空");
        emit unfreezeFindBtn();
        return;
    }
    if(findText == ""){
        return;
    }
    fkui->freezeFindBtn();
    int c = 0;
    int const length = discQuickIndex.size();
    int const keyTableFindPosSave = keyTableFindPos;
    for(int i=0; i<length; i++){
        if(keyTableFindPos >= length || keyTableFindPos < 0){
            keyTableFindPos = 0;
        }
        if(!findCheckKey()){
            c++;
        }
        keyTableFindPos++;
    }
    keyTableFindPos = keyTableFindPosSave;
    emit sendLogLText("计数: 共" + QString::number(c) + "个");
    emit unfreezeFindBtn();
}

void MainUi::on_importKeysBtn_clicked()
{
    QString importPath = QFileDialog::getOpenFileName(this, "导入数据", savePath,
                            PASSWD_FILE_FORM_ALL";;"
                            PASSWD_FILE_FORM_KCDB";;"
                            PASSWD_FILE_FORM_JSON, nullptr ,QFileDialog::DontResolveSymlinks);
    if(importPath.isEmpty()){
        return;
    }
    QFileInfo dataFileInfo(importPath);
    QString datPath = QDir::toNativeSeparators(dataFileInfo.path().replace("\\", "/") + LOGIN_PASSWD_FILE_NAME);
    if(dataFileInfo.suffix() == "kcdb"){
        if(!QFileInfo::exists(importPath+".chf")){
            log("检验文件丢失，无法读取");
            return;
        }
        loginCorrent=false;
        if(logIn != nullptr){
            delete logIn;
            logIn = nullptr;
        }
        logIn = new LogIn(nullptr, Estring(datPath));
        connect(logIn, &LogIn::setKcdbKey, this, [=](Estring k){
            if(kcdb != nullptr){
                delete kcdb;
                kcdb = nullptr;
            }
            kcdb = new Kcdb(appPath + saveName, appPath + backupName);
            kcdb->setKey(k);
        });
        connect(logIn, &LogIn::finish, this, [=](bool result, Estring pwdHash){
            if(result){
                tmpPwdHash = pwdHash;
                loginCorrent = true;
            }
            emit open2();
        });
        connect(this, &MainUi::open2, &loginLockLoop, &QEventLoop::quit);
        if(logIn->getContinueStart()){
            logIn->setContinueStart(false);
            this->setVisible(false);
            logIn->show();
            loginLockLoop.exec();
        }
        this->setVisible(true);
        // FIXME: 不安全，需要绑定login.ec与dat.ec
        if(!loginCorrent){
            log("密码错误，导入失败");
            return;
        }
        if(!checkDb(importPath)){
            log("导入失败");
            return;
        }

        if(!kcdb->readKcdb(importPath)){
            log("导入失败");
            return;
        }
        syncKeyFromMap();
        refreshKeyTW();
        currentPath = importPath;
        truePwdHash = tmpPwdHash;
        kcdb->setSaveAESKeyPath(Estring(datPath));
        log("导入成功");
    }
    else if(dataFileInfo.suffix() == "kcdj"){
        QFile dataJsonFile(importPath);
        QTextStream dataJsonStream;
        dataJsonStream.setCodec("UTF-8");
        dataJsonStream.setDevice(&dataJsonFile);
        if(!dataJsonFile.open(QIODevice::ReadOnly | QIODevice:: Text)){
            log("文件无法打开");
            return;
        }
        keyMap = KeyMapJsonEngine::jsonToKeyMap(dataJsonStream.readAll());
        refreshKeyTW();
        log("导入成功");
    }
    else{
        log("导入失败");
        return;
    }

}

void MainUi::on_keyTW_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    keyTableFindPos = currentRow;
}

void MainUi::on_autoBackupPathChB_stateChanged(int arg1)
{
    autoBackupPath = static_cast<bool>(arg1);
}

void MainUi::lockApp()
{
    loginCorrent=false;
    if(logIn != nullptr){
        delete logIn;
    }
    logIn = new LogIn();
    connect(logIn, &LogIn::setKcdbKey, this, [=](Estring k){
        if(kcdb != nullptr){
            delete kcdb;
        }
        kcdb = new Kcdb(appPath + saveName, appPath + backupName);
        kcdb->setKey(k);
    });
    connect(logIn, &LogIn::finish, this, [=](bool result, Estring pwdHash){
        Q_UNUSED(pwdHash)
        loginCorrent=result;
        emit open2();
    });
    connect(this, &MainUi::open2, &loginLockLoop, &QEventLoop::quit);
    if(logIn->getContinueStart()){
        logIn->setContinueStart(false);
        this->setVisible(false);
        logIn->show();
        loginLockLoop.exec();
        delete logIn;
        logIn = nullptr;
    }
    loginCorrent ? this->setVisible(true) : exit(0);
}

void MainUi::on_lockAppBtn_clicked()
{
    lockApp();
}

void MainUi::appStateChanged(Qt::ApplicationState state)
{
    state==Qt::ApplicationActive ?  timeLocker.stop() : timeLocker.start(timeLockerTiming);
}

void MainUi::on_lockAppTimingSB_currentTextChanged(const QString &arg1)
{
    if(timeLockerTimingMap.contains(arg1)){
        timeLockerTiming = timeLockerTimingMap[arg1];
    }
    else{
        timeLockerTiming = 300000;
    }
}

void MainUi::on_useNumberChB_stateChanged(int arg1)
{
    useNumbers = static_cast<bool>(arg1);
}

void MainUi::on_useAlphaChB_stateChanged(int arg1)
{
    useAlphas = static_cast<bool>(arg1);
}

void MainUi::on_useSymbolChB_stateChanged(int arg1)
{
    useSymbols  = static_cast<bool>(arg1);
}

void MainUi::on_gKeyBtn_clicked()
{
    ui->gResultLE->setText(randomGenerator().getVal());
}

void MainUi::on_gLengthCB_currentTextChanged(const QString &arg1)
{
    generateLength = arg1.toInt();
}

void MainUi::on_useCustomCharChB_stateChanged(int arg1)
{
    useCustom = static_cast<bool>(arg1);
}

void MainUi::on_gCopyResultBtn_clicked()
{
    cboard->setText(ui->gResultLE->text());
    log("已复制到剪切板");
}

