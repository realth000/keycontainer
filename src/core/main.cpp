#include <QDebug>
#include "commoninclude.h"
#include <QFont>
#include "debugshowoptions.h"

#ifdef DEBUG_USE_VID
#include "vld.h"
#endif

#if defined(Q_OS_WINDOWS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include <QSharedMemory>
#endif

#if defined(Q_OS_WINDOWS) && !defined(DEBUG_QML_ON_WINDOWS)
#include "gui/mainui.h"
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
#include "core/qmlimporter.h"
#include <QQmlContext>
#endif

int main(int argc, char *argv[])
{
    // 勿删
    DebugShowOptions dso;

    QFont af;
#if defined(Q_OS_WINDOWS)
    af.setFamily("Microsoft YaHei");
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    af.setFamily("DejaVu Sans Mono");
#endif
    af.setStyleStrategy(QFont::PreferAntialias);

    QCoreApplication::setApplicationName(TITLEBAR_TITLETEXT);
    QCoreApplication::setApplicationVersion(ABOUT_VERSION);
#ifndef DEBUG_QML_ON_WINDOWS
    // Windows and Linux
#if defined(Q_OS_WINDOWS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    QApplication a(argc, argv);
    a.setFont(af);
#ifndef DEBUG_DISABLE_SINGLE_DETECTION
    QSharedMemory sharedMem("key_container_shared_memory");
    if(sharedMem.attach()){
        qDebug() << "already started";
        MessageBoxExY::information("", "程序已经启动，同时只能运行一个" + QString(TITLEBAR_TITLETEXT), "退出");
        return -1;
    }
    sharedMem.create(1);
#endif
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
    const QUrl url(QStringLiteral("qrc:/resource/MainQML.qml"));
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
        MessageBoxExY::information("无法启动", "程序已在运行中");
#endif
}
