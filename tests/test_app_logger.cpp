#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QCoreApplication>
#include "../src/logging/AppLogger.h"

class TestAppLogger : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void writesToLogFile();
    void levelFiltersDebug();
    void qtMessageHandlerForwardsWarning();
};

void TestAppLogger::init()
{
    AppLogger::shutdown();
}

void TestAppLogger::cleanup()
{
    AppLogger::shutdown();
}

void TestAppLogger::writesToLogFile()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    AppLogger::init(temp.path(), true);
    QVERIFY(AppLogger::isInitialized());
    const QString logFile = AppLogger::logPath();
    QVERIFY(logFile.contains(QStringLiteral("CleanerQt.log")));

    CQ_LOG_INFO("logger_test_marker_info");
    AppLogger::flush();

    QFile file(logFile);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());
    QVERIFY(content.contains(QStringLiteral("logger_test_marker_info")));
}

void TestAppLogger::levelFiltersDebug()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    AppLogger::init(temp.path(), true);
    AppLogger::setLevel(QStringLiteral("warn"));

    CQ_LOG_DEBUG("logger_test_marker_debug_hidden");
    CQ_LOG_WARN("logger_test_marker_warn_visible");
    AppLogger::flush();

    QFile file(AppLogger::logPath());
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());
    QVERIFY(!content.contains(QStringLiteral("logger_test_marker_debug_hidden")));
    QVERIFY(content.contains(QStringLiteral("logger_test_marker_warn_visible")));
}

void TestAppLogger::qtMessageHandlerForwardsWarning()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    AppLogger::init(temp.path(), true);
    installQtMessageHandler();

    qWarning("logger_test_marker_qt_warning");
    AppLogger::flush();

    QFile file(AppLogger::logPath());
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());
    QVERIFY(content.contains(QStringLiteral("logger_test_marker_qt_warning")));
}

QTEST_MAIN(TestAppLogger)
#include "test_app_logger.moc"
