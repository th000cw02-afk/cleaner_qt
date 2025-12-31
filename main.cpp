#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include "src/CleanerController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("Disk Cleaner");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CleanerQt");
    
    qmlRegisterType<CleanerController>("Cleaner", 1, 0, "CleanerController");
    
    QQmlApplicationEngine engine;
    
    // 设置 QML 导入路径
    engine.addImportPath("qrc:/");
    
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

