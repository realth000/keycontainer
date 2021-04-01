#include "mainui.h"
#include <QObject>
#include <QApplication>
#include <QDebug>
#include "commoninclude.h"
#include <QProcess>
#include <QString>

#ifdef Q_OS_WINDOWS
#include <windows.h>
#elif defined Q_OS_LINUX
#include <QMessageBox>
#endif

int main(int argc, char *argv[])
{


    // check if running
#ifdef Q_OS_WINDOWS
    HWND pwnd = FindWindow(NULL,QString::fromUtf8(TITLEBAR_TITLETEXT).toStdWString().c_str());
    if(pwnd){
        if(IsIconic(pwnd)){
            ShowWindow(pwnd,SW_RESTORE);
        }
        SetForegroundWindow(pwnd);
        return 0;
    }
    QApplication a(argc, argv);
#elif defined Q_OS_LINUX
    QProcess p;
    p.start("pgrep " + QString(TITLEBAR_TITLETEXT));
    p.waitForFinished();
    QByteArray result = p.readAllStandardOutput();
    QApplication a(argc, argv);
    if(QString(result).count("\n") != 1){
        QMessageBox::information(NULL, QObject::tr("已启动"), TITLEBAR_TITLETEXT + QString("正在运行"));
        return 0;
    }
#endif

    MainUi MU;
    if(!MU.loginCorrent){
        return 0;
    }
    MU.show();
    return a.exec();
}
