#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include "../src/scan/ScanWorker.h"
#include "../src/scan/ScanOptions.h"

class TestScanWorker : public QObject {
    Q_OBJECT

private slots:
    void scanTempDirectory();
};

void TestScanWorker::scanTempDirectory()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QDir().mkpath(root + QStringLiteral("/a"));
    QFile f1(root + QStringLiteral("/a/one.txt"));
    QFile f2(root + QStringLiteral("/two.txt"));
    QVERIFY(f1.open(QIODevice::WriteOnly));
    QVERIFY(f2.open(QIODevice::WriteOnly));
    f1.write("hello");
    f2.write(QByteArray(200, 'x'));
    f1.close();
    f2.close();

    ScanOptions options = ScanOptions::defaultForPath(root);
    options.rootPath = root;
    options.topFileCount = 10;

    ScanWorker worker;
    ScanResult result;
    bool finished = false;
    QObject::connect(&worker, &ScanWorker::scanFinished, [&](const ScanResult& r) {
        result = r;
        finished = true;
    });
    worker.scan(options);

    QVERIFY(finished);
    QCOMPARE(result.totalFiles, 2);
    QVERIFY(result.root);
    QCOMPARE(result.root->fileCount, 2);
    QVERIFY(result.totalSize >= qint64(205));
}

QTEST_MAIN(TestScanWorker)
#include "test_scan_worker.moc"
