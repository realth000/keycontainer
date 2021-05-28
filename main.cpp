#include "mainui.h"
#include <QDebug>
#include "commoninclude.h"
#include <QFont>
#include "debugshowoptions.h"

#if defined(Q_OS_WINDOWS) && !defined(DEBUG_QML_ON_WINDOWS)
#include <QApplication>
#include <QObject>
#include <windows.h>
#include <QRegularExpression>
#include <QProcess>
#include <QString>
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <QApplication>
#include <QObject>
#include <QProcess>
#include <QString>
#elif  defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include "cml/qmlimporter.h"
#include <QQmlContext>
#endif

int main(int argc, char *argv[])
{
    debugShowOptions dso;
#ifndef DEBUG_QML_ON_WINDOWS
    // Windows and Linux
#if defined(Q_OS_WINDOWS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    // Windows only
#   if defined(Q_OS_WINDOWS)
       HWND pwnd = FindWindow(NULL,QString::fromUtf8(TITLEBAR_TITLETEXT).toStdWString().c_str());
        if(pwnd){
            if(IsIconic(pwnd)){
                ShowWindow(pwnd, SW_RESTORE);
            }
            SetForegroundWindow(pwnd);
            return 0;
        }
        QApplication a(argc, argv);
    // Linux only
#   elif defined(Q_OS_LINUX)
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
#   endif
    // Windows and Linux
    QFont af("Consolas");
    af.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(af);

    QCoreApplication::setApplicationName(TITLEBAR_TITLETEXT);
    QCoreApplication::setOrganizationName("海淀区干饭大队");
    QCoreApplication::setApplicationVersion(ABOUT_VERSION);

    MessageBoxExX w;
    MainUi MU;
    if(!MU.loginCorrent){
        return 0;
    }
    MU.show();
    return a.exec();
#endif
#endif

// Android only
#if defined(Q_OS_ANDROID) || defined(DEBUG_QML_ON_WINDOWS)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    qmlRegisterType<QmlImporter>("TH.QmlImporter", 1, 0, "QmlImporter");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/MainQML.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
#endif

// else OS
#if !defined(Q_OS_WINDOWS) && !defined(Q_OS_LINUX)
        QApplication a(argc, argv);
        MessageBoxExX t;
        t.information("无法启动", "程序已在运行中");
#endif
}
