#include "mainui.h"
#include <QObject>
#include <QApplication>
#include <QDebug>
#include "commoninclude.h"
#include <QProcess>
#include <QString>

#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <QRegularExpression>
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#elif  ifndef Q_OS_LINUX
#include "ui/messageboxex.h"
#endif

int main(int argc, char *argv[])
{


    // check if running
#ifdef Q_OS_WINDOWS
//    QString lpClassName = QString(TITLEBAR_TITLETEXT) + QString(".exe");
//    QProcess p;
//    QStringList args;
//    args <<"/c"<< "tasklist";
//    p.start("cmd", args);
//    p.waitForFinished();
//    QString(p.readAllStandardOutput()).replace("\r\n", " ").match("sd");
//    QRegularExpression r1(" +");
//    QRegularExpression r2("[1-9,]* K");
//    QStringList pid_list = (QString(p.readAllStandardOutput())).replace("\r\n", " ").replace(r1," ").split(r2,QString::SkipEmptyParts);
//    qDebug() << pid_list;
//    return 3;
//    int range = pid_list.length()-1;
//    DWORD old_pid1=0;
//    DWORD old_pid2=0;
//    for(int i=0; i<range; i++){
//        if(pid_list[i] == "Key" && pid_list[i+1]=="Container.exe"){
//            old_pid1 = pid_list[i+2].replace(",","").toUInt();
//        }
//    }
//    qDebug() << "cmd result" << list.indexOf("Key Container.exe");

//    qDebug() << "cmd result" << old_pid1 << old_pid2;
    // FIXME: 受同名窗口影响
       HWND pwnd = FindWindow(NULL,QString::fromUtf8(TITLEBAR_TITLETEXT).toStdWString().c_str());
        if(pwnd){
//            DWORD checkProcessID = 0;//12744
//            GetWindowThreadProcessId(pwnd, &checkProcessID);
//            qDebug() << checkProcessID;
            if(IsIconic(pwnd)){
                ShowWindow(pwnd, SW_RESTORE);
            }
            SetForegroundWindow(pwnd);
            return 0;
        }
    QApplication a(argc, argv);
#elif defined Q_OS_LINUX
     QProcess p;
    // 特殊符号，start()会有\n，execute()的\n会解释为换行，execute()的输出直接输出到程序控制台，没法捕获
    p.start("pgrep " + QString(TITLEBAR_TITLETEXT));
    p.waitForFinished();
    QByteArray all_pids = p.readAllStandardOutput();
    QApplication a(argc, argv);
    if(QString(all_pids).count("\n") != 1){
        QProcess q;
        // 带管道的需要使用(QString programPath, QStringList args);
        QStringList t;
        // grep的是.pro文件中的程序名TARGET，对wmctrl的x选项服务，防止错误激活名字为KeyContainer的其他同名窗口
        // 经验证，最小化时、在其他桌面时也可以正确激活
        t << "-c" << "wmctrl -lx | grep \" KeyContainer.KeyContainer \" | awk -F \" \" '{print $1}' | xargs wmctrl -ia";
        // q.start("/bin/bash", t);
        q.execute("/bin/bash", t);
        return 0;
    }
#else
    QApplication a(argc, argv);
    MessageBoxEx t;
    t.information("无法启动", "程序已在运行中");
#endif

    MainUi MU;
    if(!MU.loginCorrent){
        return 0;
    }
    MU.show();
    return a.exec();
}
