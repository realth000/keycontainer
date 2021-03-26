#include "mainui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainUi MU;
    MU.show();
    return a.exec();
}
