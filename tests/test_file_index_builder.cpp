#include <QtTest>
#include "../src/models/FileIndexBuilder.h"
#include "../src/models/FileIndexEntry.h"

class TestFileIndexBuilder : public QObject {
    Q_OBJECT

private slots:
    void addAndCount();
    void takeEntriesClears();
    void clearResets();
};

void TestFileIndexBuilder::addAndCount()
{
    FileIndexBuilder builder;
    FileIndexEntry entry;
    entry.path = QStringLiteral("C:/a/file.txt");
    entry.logicalSize = 100;
    entry.extension = QStringLiteral(".txt");
    builder.addFile(entry);
    QCOMPARE(builder.count(), 1);
    const QVector<FileIndexEntry> entries = builder.entries();
    QCOMPARE(entries.size(), 1);
    QCOMPARE(entries.first().path, entry.path);
}

void TestFileIndexBuilder::takeEntriesClears()
{
    FileIndexBuilder builder;
    FileIndexEntry entry;
    entry.path = QStringLiteral("C:/b.dat");
    builder.addFile(entry);
    QVector<FileIndexEntry> taken = builder.takeEntries();
    QCOMPARE(taken.size(), 1);
    QCOMPARE(builder.count(), 0);
}

void TestFileIndexBuilder::clearResets()
{
    FileIndexBuilder builder;
    FileIndexEntry entry;
    entry.path = QStringLiteral("C:/c.bin");
    builder.addFile(entry);
    builder.clear();
    QCOMPARE(builder.count(), 0);
    QVERIFY(builder.hardlinkRegistry() != nullptr);
}

QTEST_MAIN(TestFileIndexBuilder)
#include "test_file_index_builder.moc"
