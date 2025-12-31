#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QDebug>
#include <QLoggingCategory>
#include <QQmlError>
#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif
#include "src/CleanerController.h"

int main(int argc, char *argv[])
{
    // 启用控制台输出并设置 UTF-8 编码（用于调试）
    #ifdef Q_OS_WIN
    // 设置控制台代码页为 UTF-8（必须在任何输出之前）
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    // 尝试附加到父进程控制台
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        // 如果附加失败，创建新控制台
        AllocConsole();
    }
    
    // 重定向标准输出到控制台
    FILE* pCout;
    FILE* pCerr;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    freopen_s(&pCerr, "CONOUT$", "w", stderr);
    
    // 再次设置代码页确保生效
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #endif
    
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("Disk Cleaner");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CleanerQt");
    
    qDebug() << "Application starting...";
    
    // 注册 QML 类型
    qmlRegisterType<CleanerController>("Cleaner", 1, 0, "CleanerController");
    qDebug() << "CleanerController registered";
    
    QQmlApplicationEngine engine;
    
    // 设置 QML 导入路径
    // 注意：qmldir 文件在 qrc:/components/ 下，模块名是 Components
    // QML 引擎会在导入路径下查找 Components/qmldir 或 components/qmldir
    engine.addImportPath("qrc:/");
    
    // 为了确保能找到模块，也添加 components 路径
    // 但主要导入路径应该是 qrc:/，这样 QML 引擎会在 qrc:/components/qmldir 找到它
    
    // 输出所有导入路径用于调试
    qDebug() << "QML Import Paths:" << engine.importPathList();
    
    // 测试组件路径和文件
    qDebug() << "=== Checking Resource Files ===";
    QFileInfo mainQml(":/main.qml");
    qDebug() << ":/main.qml exists:" << mainQml.exists() << "size:" << (mainQml.exists() ? mainQml.size() : 0);
    
    QFileInfo qmldir(":/components/qmldir");
    qDebug() << ":/components/qmldir exists:" << qmldir.exists() << "size:" << (qmldir.exists() ? qmldir.size() : 0);
    
    QFileInfo diskSelector(":/components/DiskSelector.qml");
    qDebug() << ":/components/DiskSelector.qml exists:" << diskSelector.exists();
    
    // 尝试读取 qmldir 内容
    if (qmldir.exists()) {
        QFile file(":/components/qmldir");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            qDebug() << "qmldir content:";
            while (!in.atEnd()) {
                qDebug() << "  " << in.readLine();
            }
            file.close();
        }
    }
    qDebug() << "================================";
    
    // 注意：resources.qrc 中 prefix 是 "/"，文件路径是相对于 qrc 文件的
    // 由于 resources.qrc 在 qml/ 目录下，所以路径应该是 qrc:/main.qml
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    qDebug() << "Loading QML from:" << url.toString();
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to load QML file:" << objUrl.toString();
            QCoreApplication::exit(-1);
        } else if (obj) {
            qDebug() << "QML file loaded successfully:" << objUrl.toString();
        }
    }, Qt::QueuedConnection);
    
    // 连接错误信号 - 输出详细的错误信息
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, [](const QList<QQmlError> &warnings) {
        qWarning() << "=== QML Warnings/Errors ===";
        for (const auto &error : warnings) {
            qWarning() << "Error:" << error.toString();
            qWarning() << "  File:" << error.url().toString();
            qWarning() << "  Line:" << error.line();
            qWarning() << "  Column:" << error.column();
            qWarning() << "  Description:" << error.description();
        }
        qWarning() << "===========================";
    });
    
    engine.load(url);
    
    // 检查加载后的错误
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "=== No root objects created! ===";
        qCritical() << "This usually means QML file failed to load.";
        
        // 检查资源文件
        QFileInfo mainQmlCheck(":/main.qml");
        qCritical() << "Checking resource file :/main.qml exists:" << mainQmlCheck.exists();
        
        QFileInfo qmldirCheck(":/components/qmldir");
        qCritical() << "Checking qmldir file :/components/qmldir exists:" << qmldirCheck.exists();
        
        return -1;
    }
    
    qDebug() << "Application started successfully";
    return app.exec();
}

