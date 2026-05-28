#include "CliRunner.h"
#include "../scan/ScanOptions.h"
#include "../scan/ScanWorker.h"
#include "../scan/NtfsMftScanner_win.h"
#include "../scan/ReportExporter.h"
#include "../settings/AppSettings.h"
#include "../logging/AppLogger.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

static void initCliLogging(QCoreApplication& app, QCommandLineParser& parser)
{
    QCommandLineOption verboseOption(QStringLiteral("verbose"), QStringLiteral("Enable debug logging"));
    parser.addOption(verboseOption);
    parser.process(app.arguments());

    AppLogger::init(app.applicationDirPath(), AppSettings::instance().logToFile());
    installQtMessageHandler();
    if (parser.isSet(QStringLiteral("verbose"))) {
        AppLogger::setLevel(QStringLiteral("debug"));
    } else {
        AppLogger::setLevelFromSettings();
    }
}

bool CliRunner::tryRun(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        const QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg == QStringLiteral("--scan") || arg == QStringLiteral("-s")) {
            QCoreApplication app(argc, argv);
            QCommandLineParser parser;
            parser.setApplicationDescription(QStringLiteral("CleanerQt CLI scanner"));
            parser.addHelpOption();
            QCommandLineOption scanOption(QStringList() << QStringLiteral("s") << QStringLiteral("scan"),
                                          QStringLiteral("Directory to scan"), QStringLiteral("path"));
            QCommandLineOption outputOption(QStringLiteral("output"), QStringLiteral("Output file"),
                                            QStringLiteral("file"));
            QCommandLineOption formatOption(QStringLiteral("format"), QStringLiteral("csv or html"),
                                            QStringLiteral("format"), QStringLiteral("csv"));
            QCommandLineOption mftOption(QStringLiteral("mft"), QStringLiteral("Prefer MFT scan"));
            parser.addOption(scanOption);
            parser.addOption(outputOption);
            parser.addOption(formatOption);
            parser.addOption(mftOption);
            initCliLogging(app, parser);

            const QString path = parser.value(QStringLiteral("scan"));
            if (path.isEmpty()) {
                CQ_LOG_ERROR("Missing scan path");
                AppLogger::shutdown();
                return true;
            }

            CQ_LOG_INFO(QStringLiteral("CLI scan: %1").arg(path).toStdString());

            ScanOptions options = ScanOptions::defaultForPath(path);
            AppSettings::instance().applyToScanOptions(options);
            options.rootPath = path;

            ScanResult result;
            QString mftReason;
            if (parser.isSet(QStringLiteral("mft")) && NtfsMftScanner::canUseMftScan(path, mftReason)) {
                result = NtfsMftScanner::scan(options, [] { return false; }, nullptr);
            } else {
                ScanWorker worker;
                bool done = false;
                QObject::connect(&worker, &ScanWorker::scanFinished, [&](const ScanResult& r) {
                    result = r;
                    done = true;
                });
                worker.scan(options);
                if (!done) {
                    CQ_LOG_ERROR("Scan failed");
                    AppLogger::shutdown();
                    return true;
                }
            }

            const QString output = parser.value(QStringLiteral("output"));
            const QString format = parser.value(QStringLiteral("format"));
            QString error;
            bool ok = false;
            if (format == QStringLiteral("html")) {
                ok = ReportExporter::exportHtml(output.isEmpty() ? QStringLiteral("scan.html") : output,
                                                  result.root, result.topFiles, result.totalSize, &error);
            } else {
                ok = ReportExporter::exportCsv(output.isEmpty() ? QStringLiteral("scan.csv") : output,
                                                 result.root, result.topFiles, &error);
            }
            if (!ok) {
                CQ_LOG_ERROR(error.toStdString());
            } else {
                CQ_LOG_INFO(QStringLiteral("Exported %1 files").arg(result.totalFiles).toStdString());
            }
            AppLogger::shutdown();
            return true;
        }
        if (arg == QStringLiteral("--import-csv")) {
            QCoreApplication app(argc, argv);
            QCommandLineParser parser;
            parser.addOption({QStringLiteral("import-csv"), QStringLiteral("CSV file")});
            initCliLogging(app, parser);
            const QString csvPath = parser.value(QStringLiteral("import-csv"));
            QFile file(csvPath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                CQ_LOG_ERROR("Cannot open CSV");
                AppLogger::shutdown();
                return true;
            }
            int count = 0;
            while (!file.atEnd()) {
                const QString line = QString::fromUtf8(file.readLine()).trimmed();
                if (!line.isEmpty() && !line.startsWith(QLatin1Char('#'))) {
                    count++;
                }
            }
            CQ_LOG_INFO(QStringLiteral("CSV rows: %1").arg(count).toStdString());
            AppLogger::shutdown();
            return true;
        }
    }
    return false;
}
