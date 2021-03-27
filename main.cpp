#include "mainui.h"
#include <QObject>
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainUi MU;
    MU.show();
    if(!MU.loginCorrent){
        return 0;
    }
    return a.exec();
}
