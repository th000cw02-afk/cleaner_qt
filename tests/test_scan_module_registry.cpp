#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include "../src/scan/modules/ScanModule.h"

class TestScanModuleRegistry : public QObject {
    Q_OBJECT

private slots:
    void registryMetadata();
    void bigFilesModule();
    void tempFilesModule();
    void zeroLengthModule();
    void stubModulesEmpty();
};

void TestScanModuleRegistry::registryMetadata()
{
    ScanModuleRegistry& registry = ScanModuleRegistry::instance();
    const QStringList ids = registry.moduleIds();
    QCOMPARE(ids.size(), 12);
    for (const QString& id : ids) {
        QVERIFY(!registry.displayNameFor(id).isEmpty());
    }
    QCOMPARE(registry.displayNameFor(QStringLiteral("unknown_module")),
             QStringLiteral("unknown_module"));
    QVERIFY(registry.runModule(QStringLiteral("unknown_module"), {}).isEmpty());
}

void TestScanModuleRegistry::bigFilesModule()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QFile small(root + QStringLiteral("/small.bin"));
    QFile large(root + QStringLiteral("/large.bin"));
    QVERIFY(small.open(QIODevice::WriteOnly));
    QVERIFY(large.open(QIODevice::WriteOnly));
    small.write(QByteArray(100, 's'));
    large.write(QByteArray(5000, 'L'));
    small.close();
    large.close();

    CleanupScanOptions opts;
    opts.rootPath = root;
    opts.minFileSize = 1024;
    opts.maxResults = 100;
    const QVariantList results =
        ScanModuleRegistry::instance().runModule(QStringLiteral("bigFiles"), opts);
    QCOMPARE(results.size(), 1);
    QCOMPARE(results.first().toMap().value(QStringLiteral("type")).toString(),
             QStringLiteral("big_file"));
}

void TestScanModuleRegistry::tempFilesModule()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QFile tmp(root + QStringLiteral("/cache.tmp"));
    QFile plain(root + QStringLiteral("/readme.txt"));
    QVERIFY(tmp.open(QIODevice::WriteOnly));
    QVERIFY(plain.open(QIODevice::WriteOnly));
    tmp.write("x");
    plain.write("y");
    tmp.close();
    plain.close();

    CleanupScanOptions opts;
    opts.rootPath = root;
    opts.maxResults = 100;
    const QVariantList results =
        ScanModuleRegistry::instance().runModule(QStringLiteral("tempFiles"), opts);
    QCOMPARE(results.size(), 1);
    QVERIFY(results.first().toMap().value(QStringLiteral("path")).toString().endsWith(
        QStringLiteral(".tmp")));
}

void TestScanModuleRegistry::zeroLengthModule()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QFile empty(root + QStringLiteral("/empty.dat"));
    QVERIFY(empty.open(QIODevice::WriteOnly));
    empty.close();

    CleanupScanOptions opts;
    opts.rootPath = root;
    opts.maxResults = 100;
    const QVariantList results =
        ScanModuleRegistry::instance().runModule(QStringLiteral("zeroLength"), opts);
    QVERIFY(results.size() >= 1);
}

void TestScanModuleRegistry::stubModulesEmpty()
{
    CleanupScanOptions opts;
    opts.rootPath = QStringLiteral("C:/");
    ScanModuleRegistry& registry = ScanModuleRegistry::instance();
    QVERIFY(registry.runModule(QStringLiteral("brokenLinks"), opts).isEmpty());
    QVERIFY(registry.runModule(QStringLiteral("dupArchives"), opts).isEmpty());
    QVERIFY(registry.runModule(QStringLiteral("orphanedAppData"), opts).isEmpty());
}

QTEST_MAIN(TestScanModuleRegistry)
#include "test_scan_module_registry.moc"
