#include <QtTest>
#include "../src/models/TopFilesTracker.h"
#include "../src/FileInfo.h"

class TestTopFilesTracker : public QObject {
    Q_OBJECT

private slots:
    void equalSizeDoesNotReplace();
    void duplicatePathsAllowed();
    void maxCountZeroAppendsAll();
    void sortedDescending();
};

void TestTopFilesTracker::equalSizeDoesNotReplace()
{
    TopFilesTracker tracker(2);
    tracker.consider(FileInfo(QStringLiteral("/a"), 10, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/b"), 20, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/c"), 10, QDateTime(), false));

    const auto top = tracker.sortedTopFiles();
    QCOMPARE(top.size(), 2);
    QCOMPARE(top.at(0).size, qint64(20));
    QCOMPARE(top.at(1).size, qint64(10));
    QCOMPARE(top.at(1).path, QStringLiteral("/a"));
}

void TestTopFilesTracker::duplicatePathsAllowed()
{
    TopFilesTracker tracker(3);
    tracker.consider(FileInfo(QStringLiteral("/same"), 5, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/same"), 15, QDateTime(), false));
    QCOMPARE(tracker.sortedTopFiles().size(), 2);
}

void TestTopFilesTracker::maxCountZeroAppendsAll()
{
    TopFilesTracker tracker(0);
    tracker.consider(FileInfo(QStringLiteral("/a"), 1, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/b"), 2, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/c"), 3, QDateTime(), false));
    QCOMPARE(tracker.sortedTopFiles().size(), 0);
}

void TestTopFilesTracker::sortedDescending()
{
    TopFilesTracker tracker(5);
    tracker.consider(FileInfo(QStringLiteral("/a"), 5, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/b"), 50, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/c"), 25, QDateTime(), false));
    const auto top = tracker.sortedTopFiles();
    QCOMPARE(top.size(), 3);
    QVERIFY(top.at(0).size >= top.at(1).size);
    QVERIFY(top.at(1).size >= top.at(2).size);
}

QTEST_MAIN(TestTopFilesTracker)
#include "test_top_files_tracker.moc"
