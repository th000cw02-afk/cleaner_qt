#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QDateTime>
#include "../src/models/FolderNode.h"
#include "../src/models/TopFilesTracker.h"
#include "../src/scan/ScanOptions.h"
#include "../src/FileInfo.h"

class TestFolderTree : public QObject {
    Q_OBJECT

private slots:
    void folderAggregation();
    void nestedFolderAggregation();
    void sortChildrenBySize();
    void topFilesTracker();
    void topFilesTrackerMaxOne();
    void scanOptionsExclude();
};

void TestFolderTree::folderAggregation()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QDir().mkpath(root + "/a");
    QDir().mkpath(root + "/b");
    QFile f1(root + "/a/file1.txt");
    QVERIFY(f1.open(QIODevice::WriteOnly));
    f1.write("hello");
    f1.close();
    QFile f2(root + "/b/file2.txt");
    QVERIFY(f2.open(QIODevice::WriteOnly));
    f2.write(QByteArray(300, 'x'));
    f2.close();

    FolderTreeBuilder builder(root);
    builder.addFileSize(root + QStringLiteral("/a/file1.txt"), f1.size());
    builder.addFileSize(root + QStringLiteral("/b/file2.txt"), f2.size());

    auto treeRoot = builder.root();
    QVERIFY(treeRoot);
    QCOMPARE(treeRoot->size, qint64(305));
    QCOMPARE(treeRoot->fileCount, 2);
    QVERIFY(treeRoot->children.size() >= 2);
}

void TestFolderTree::nestedFolderAggregation()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QDir().mkpath(root + QStringLiteral("/a/b"));
    QFile f(root + QStringLiteral("/a/b/deep.txt"));
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(QByteArray(100, 'z'));
    f.close();

    FolderTreeBuilder builder(root);
    builder.addFileSize(root + QStringLiteral("/a/b/deep.txt"), f.size(), f.size(), QDateTime::currentDateTime());

    auto treeRoot = builder.root();
    QVERIFY(treeRoot);
    QCOMPARE(treeRoot->size, qint64(100));
    QCOMPARE(treeRoot->fileCount, 1);
}

void TestFolderTree::sortChildrenBySize()
{
    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    auto small = std::make_shared<FolderNode>(QStringLiteral("C:/root/small"));
    small->size = 10;
    auto large = std::make_shared<FolderNode>(QStringLiteral("C:/root/large"));
    large->size = 1000;
    root->addChild(small);
    root->addChild(large);
    root->sortChildrenBySize();
    QCOMPARE(root->children.first()->name, QStringLiteral("large"));
}

void TestFolderTree::topFilesTracker()
{
    TopFilesTracker tracker(2);
    tracker.consider(FileInfo(QStringLiteral("/a"), 10, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/b"), 20, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/c"), 30, QDateTime(), false));

    const auto top = tracker.sortedTopFiles();
    QCOMPARE(top.size(), 2);
    QCOMPARE(top.at(0).size, qint64(30));
    QCOMPARE(top.at(1).size, qint64(20));
}

void TestFolderTree::topFilesTrackerMaxOne()
{
    TopFilesTracker tracker(1);
    tracker.consider(FileInfo(QStringLiteral("/a"), 10, QDateTime(), false));
    tracker.consider(FileInfo(QStringLiteral("/b"), 20, QDateTime(), false));
    const auto top = tracker.sortedTopFiles();
    QCOMPARE(top.size(), 1);
    QCOMPARE(top.at(0).size, qint64(20));
}

void TestFolderTree::scanOptionsExclude()
{
    ScanOptions opts = ScanOptions::defaultForPath(QStringLiteral("C:/"));
    QVERIFY(opts.shouldExcludeDirName(QStringLiteral("$Recycle.Bin")));
    QVERIFY(opts.shouldExcludePath(QStringLiteral("C:/$Recycle.Bin/foo")));
    QVERIFY(!opts.shouldExcludePath(QStringLiteral("C:/Users/test")));
}

QTEST_MAIN(TestFolderTree)
#include "test_folder_tree.moc"
