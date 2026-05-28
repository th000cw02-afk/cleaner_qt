#include <QtTest>
#include <QTemporaryDir>
#include "../src/scan/ScanOptions.h"

class TestScanOptions : public QObject {
    Q_OBJECT

private slots:
    void defaultExcludes();
    void customExcludeDirName();
    void minFileSizeDefault();
    void isOnScanVolumeWithTempRoot();
};

void TestScanOptions::defaultExcludes()
{
    ScanOptions opts = ScanOptions::defaultForPath(QStringLiteral("C:/"));
    QVERIFY(opts.shouldExcludeDirName(QStringLiteral("$Recycle.Bin")));
    QVERIFY(opts.shouldExcludePath(QStringLiteral("C:/$Recycle.Bin/foo")));
    QVERIFY(!opts.shouldExcludePath(QStringLiteral("C:/Users/test")));
}

void TestScanOptions::customExcludeDirName()
{
    ScanOptions opts = ScanOptions::defaultForPath(QStringLiteral("D:/"));
    opts.excludeDirNames.append(QStringLiteral("node_modules"));
    QVERIFY(opts.shouldExcludeDirName(QStringLiteral("node_modules")));
    QVERIFY(opts.shouldExcludePath(QStringLiteral("D:/proj/node_modules/pkg")));
}

void TestScanOptions::minFileSizeDefault()
{
    ScanOptions opts = ScanOptions::defaultForPath(QStringLiteral("C:/"));
    QCOMPARE(opts.minFileSize, qint64(0));
    opts.minFileSize = 1024;
    QCOMPARE(opts.minFileSize, qint64(1024));
}

void TestScanOptions::isOnScanVolumeWithTempRoot()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    ScanOptions opts = ScanOptions::defaultForPath(temp.path());
    QVERIFY(!opts.volumeRoot.isEmpty());
    QVERIFY(opts.isOnScanVolume(temp.filePath(QStringLiteral("subdir/file.txt"))));
    opts.stopAtMountPoints = false;
    QVERIFY(opts.isOnScanVolume(QStringLiteral("Z:/other/file.txt")));
}

QTEST_MAIN(TestScanOptions)
#include "test_scan_options.moc"
