#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include "../src/scan/CleanupScanners.h"

class TestCleanupScanners : public QObject {
    Q_OBJECT

private slots:
    void findEmptyDirectories();
    void findDuplicateCandidates();
};

void TestCleanupScanners::findEmptyDirectories()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    QDir().mkpath(root + QStringLiteral("/empty_sub"));
    QDir().mkpath(root + QStringLiteral("/has_file"));
    QFile f(root + QStringLiteral("/has_file/x.txt"));
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();

    CleanupScanOptions opts;
    opts.rootPath = root;
    opts.maxResults = 100;
    const QVariantList results = CleanupScanners::findEmptyDirectories(opts);
    bool foundEmpty = false;
    for (const QVariant& v : results) {
        const QString path = v.toMap().value(QStringLiteral("path")).toString();
        if (path.endsWith(QStringLiteral("empty_sub"), Qt::CaseInsensitive)) {
            foundEmpty = true;
            break;
        }
    }
    QVERIFY(foundEmpty);
}

void TestCleanupScanners::findDuplicateCandidates()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString root = temp.path();
    const QByteArray payload(5000, 'D');
    QFile f1(root + QStringLiteral("/a.bin"));
    QFile f2(root + QStringLiteral("/b.bin"));
    QVERIFY(f1.open(QIODevice::WriteOnly));
    QVERIFY(f2.open(QIODevice::WriteOnly));
    f1.write(payload);
    f2.write(payload);
    f1.close();
    f2.close();

    CleanupScanOptions opts;
    opts.rootPath = root;
    opts.minFileSize = 1024;
    opts.maxResults = 100;
    const QVariantList results = CleanupScanners::findDuplicateCandidates(opts);
    QVERIFY(results.size() >= 1);
    const QVariantMap group = results.first().toMap();
    const QVariantList paths = group.value(QStringLiteral("paths")).toList();
    QVERIFY(paths.size() >= 2);
}

QTEST_MAIN(TestCleanupScanners)
#include "test_cleanup_scanners.moc"
