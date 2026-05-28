#include <QtTest>
#include <QTemporaryDir>
#include "../src/models/ScanSnapshot.h"
#include "../src/models/FolderNode.h"
#include "../src/models/FileIndexEntry.h"
#include "../src/scan/ScanWorker.h"

class TestScanSnapshot : public QObject {
    Q_OBJECT

private slots:
    void roundTripPreservesData();
    void saveFailsWithoutRoot();
};

void TestScanSnapshot::roundTripPreservesData()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());

    ScanResult original;
    original.scanPath = QStringLiteral("C:/scan");
    original.totalSize = 1500;
    original.totalAllocatedSize = 1600;
    original.totalFiles = 2;
    original.directoriesScanned = 3;
    original.usedMft = false;

    original.root = std::make_shared<FolderNode>(QStringLiteral("C:/scan"));
    auto child = std::make_shared<FolderNode>(QStringLiteral("C:/scan/sub"));
    child->size = 1500;
    child->fileCount = 2;
    original.root->addChild(child);
    original.root->size = 1500;
    original.root->fileCount = 2;

    FileIndexEntry entry;
    entry.path = QStringLiteral("C:/scan/sub/file.dat");
    entry.logicalSize = 1500;
    entry.extension = QStringLiteral(".dat");
    original.fileIndex.append(entry);

    original.topFiles.append(
        FileInfo(QStringLiteral("C:/scan/sub/file.dat"), 1500, QDateTime::currentDateTime(), false));

    const QString path = temp.filePath(QStringLiteral("snap.cqtscan"));
    QString error;
    QVERIFY(ScanSnapshot::save(path, original, &error));

    ScanResult loaded;
    QVERIFY(ScanSnapshot::load(path, &loaded, &error));
    QCOMPARE(loaded.scanPath, original.scanPath);
    QCOMPARE(loaded.totalSize, original.totalSize);
    QCOMPARE(loaded.totalAllocatedSize, original.totalAllocatedSize);
    QCOMPARE(loaded.totalFiles, original.totalFiles);
    QCOMPARE(loaded.fileIndex.size(), 1);
    QVERIFY(loaded.root);
    QCOMPARE(loaded.root->children.size(), 1);
    QCOMPARE(loaded.topFiles.size(), 1);
}

void TestScanSnapshot::saveFailsWithoutRoot()
{
    ScanResult empty;
    QString error;
    QVERIFY(!ScanSnapshot::save(QStringLiteral("x.cqtscan"), empty, &error));
    QVERIFY(!error.isEmpty());
}

QTEST_MAIN(TestScanSnapshot)
#include "test_scan_snapshot.moc"
