#include "mainui.h"
#include <QObject>
#include <QApplication>
#include <QDebug>
#include "commoninclude.h"
#include <QProcess>

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    // check if running
#ifdef Q_OS_WINDOWS
    HWND pwnd = FindWindow(NULL,QString::fromUtf8(TITLEBAR_TITLETEXT).toStdWString().c_str());
    if(pwnd){
        SetForegroundWindow(pwnd);
        return 0;
    }
#elif defined Q_OS_LINUX
    QProcess p;
    p.start("pgrep " + QString(TITLEBAR_TITLETEXT) + " | ");
#endif


    QApplication a(argc, argv);
    MainUi MU;
    if(!MU.loginCorrent){
        return 0;
    }
    MU.show();
    return a.exec();
}
