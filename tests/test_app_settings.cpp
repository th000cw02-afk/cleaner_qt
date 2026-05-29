#include <QtTest>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include "../src/settings/AppSettings.h"
#include "../src/scan/ScanOptions.h"

class TestAppSettings : public QObject {
    Q_OBJECT

private slots:
    void cleanupTestCase();
    void defaultExcludeDirNames();
    void applyToScanOptionsMapsFields();
    void setThemeNormalizes();
    void iniRoundTrip();
    void loadKeepsInvalidThemeUntilSetTheme();
    void logLevelNormalization();
};

void TestAppSettings::cleanupTestCase()
{
    const QString ini = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("CleanerQt.ini"));
    QFile::remove(ini);
}

void TestAppSettings::applyToScanOptionsMapsFields()
{
    AppSettings& settings = AppSettings::instance();
    settings.setMinFileSize(4096);
    settings.setExcludeDirNames({QStringLiteral("custom_exclude")});
    settings.setStopAtMountPoints(false);
    settings.setSkipReparseAndCloud(false);

    ScanOptions options = ScanOptions::defaultForPath(QStringLiteral("C:/"));
    settings.applyToScanOptions(options);

    QCOMPARE(options.minFileSize, qint64(4096));
    QCOMPARE(options.excludeDirNames, QStringList{QStringLiteral("custom_exclude")});
    QCOMPARE(options.stopAtMountPoints, false);
    QCOMPARE(options.skipReparseAndCloud, false);
}

void TestAppSettings::setThemeNormalizes()
{
    AppSettings& settings = AppSettings::instance();
    settings.setTheme(QStringLiteral("dark"));
    QCOMPARE(settings.theme(), QStringLiteral("dark"));
    settings.setTheme(QStringLiteral("neon"));
    QCOMPARE(settings.theme(), QStringLiteral("light"));
}

void TestAppSettings::defaultExcludeDirNames()
{
    AppSettings& settings = AppSettings::instance();
    QVERIFY(settings.excludeDirNames().contains(QStringLiteral("$Recycle.Bin")));
}

void TestAppSettings::iniRoundTrip()
{
    AppSettings& settings = AppSettings::instance();
    settings.setPortableMode(true);
    settings.setMinFileSize(88888);
    settings.setLogLevel(QStringLiteral("warn"));
    settings.setLogToFile(false);
    settings.setLastScanPath(QStringLiteral("D:/test_scan"));
    settings.save();

    settings.setMinFileSize(0);
    settings.setLogLevel(QStringLiteral("info"));
    settings.setLogToFile(true);
    settings.setLastScanPath(QString());
    settings.load();

    QCOMPARE(settings.minFileSize(), qint64(88888));
    QCOMPARE(settings.logLevel(), QStringLiteral("warn"));
    QCOMPARE(settings.logToFile(), false);
    QCOMPARE(settings.lastScanPath(), QStringLiteral("D:/test_scan"));
}

void TestAppSettings::loadKeepsInvalidThemeUntilSetTheme()
{
    AppSettings& settings = AppSettings::instance();
    settings.setPortableMode(true);
    settings.save();
    const QString ini =
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("CleanerQt.ini"));
    QSettings writer(ini, QSettings::IniFormat);
    writer.setValue(QStringLiteral("theme"), QStringLiteral("neon"));
    writer.sync();

    settings.load();
    QCOMPARE(settings.theme(), QStringLiteral("neon"));
    settings.setTheme(QStringLiteral("neon"));
    QCOMPARE(settings.theme(), QStringLiteral("light"));
}

void TestAppSettings::logLevelNormalization()
{
    AppSettings& settings = AppSettings::instance();
    settings.setLogLevel(QStringLiteral("warning"));
    QCOMPARE(settings.logLevel(), QStringLiteral("warn"));
    settings.setLogLevel(QStringLiteral("verbose"));
    QCOMPARE(settings.logLevel(), QStringLiteral("info"));
}

QTEST_MAIN(TestAppSettings)
#include "test_app_settings.moc"
