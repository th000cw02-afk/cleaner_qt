#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlError>
#include <QDir>

#include "src/AnalyzerController.h"
#include "src/models/FolderTreeModel.h"
#include "src/models/FileIndexModel.h"
#include "src/models/TreemapLayout.h"
#include "src/ui/ThemeManager.h"
#include "src/settings/AppSettings.h"
#include "src/scan/ScanWorker.h"
#include "src/cli/CliRunner.h"
#include "src/logging/AppLogger.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

static void showFailure(const QString& message)
{
    CQ_LOG_ERROR(message.toStdString());
#ifdef _WIN32
    MessageBoxW(nullptr, reinterpret_cast<LPCWSTR>(message.utf16()),
                L"CleanerQt", MB_ICONERROR | MB_OK);
#endif
}

static void logQmlErrors(const QList<QQmlError>& errors)
{
    for (const QQmlError& error : errors) {
        qWarning().noquote() << error.toString();
    }
}

int main(int argc, char* argv[])
{
    if (CliRunner::tryRun(argc, argv)) {
        return 0;
    }

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Disk Space Analyzer"));
    app.setApplicationVersion(QStringLiteral("2.0.0"));
    app.setOrganizationName(QStringLiteral("CleanerQt"));

    AppLogger::init(app.applicationDirPath(), AppSettings::instance().logToFile());
    installQtMessageHandler();
    CQ_LOG_INFO(QStringLiteral("CleanerQt GUI starting").toStdString());

    qmlRegisterType<AnalyzerController>("Analyzer", 1, 0, "AnalyzerController");
    qmlRegisterType<FolderTreeModel>("Analyzer", 1, 0, "FolderTreeModel");
    qmlRegisterType<FileIndexModel>("Analyzer", 1, 0, "FileIndexModel");
    qmlRegisterType<TreemapLayout>("Analyzer", 1, 0, "TreemapLayout");
    qmlRegisterType<ThemeManager>("Analyzer", 1, 0, "ThemeManager");
    qRegisterMetaType<ScanResult>("ScanResult");
    qmlRegisterSingletonInstance("Analyzer", 1, 0, "AppSettings", &AppSettings::instance());

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError>& warnings) { logQmlErrors(warnings); });
    engine.addImportPath(QStringLiteral("qrc:/"));

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        showFailure(QStringLiteral("无法加载界面 (qrc:/main.qml)。请查看 logs/CleanerQt.log"));
        return -1;
    }

    return app.exec();
}
