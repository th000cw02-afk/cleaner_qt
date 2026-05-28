#include <QtTest>
#include "../src/models/ExtensionStatsAggregator.h"

class TestExtensionStats : public QObject {
    Q_OBJECT

private slots:
    void aggregatesByExtension();
    void sortsByTotalSize();
    void noExtensionBucket();
    void clearRemovesStats();
};

void TestExtensionStats::aggregatesByExtension()
{
    ExtensionStatsAggregator agg;
    agg.addFile(QStringLiteral("C:/a/file.txt"), 100);
    agg.addFile(QStringLiteral("C:/b/file.txt"), 200);
    agg.addFile(QStringLiteral("C:/c/data.bin"), 50);

    const QVariantList list = agg.toVariantList();
    QVERIFY(list.size() >= 2);
    QCOMPARE(list.first().toMap().value(QStringLiteral("extension")).toString(), QStringLiteral(".txt"));
}

void TestExtensionStats::sortsByTotalSize()
{
    ExtensionStatsAggregator agg;
    agg.addFile(QStringLiteral("C:/a.bin"), 50);
    agg.addFile(QStringLiteral("C:/b.txt"), 200);
    agg.addFile(QStringLiteral("C:/c.txt"), 100);
    const QVariantList list = agg.toVariantList();
    QCOMPARE(list.first().toMap().value(QStringLiteral("extension")).toString(), QStringLiteral(".txt"));
    QCOMPARE(list.first().toMap().value(QStringLiteral("totalSize")).toLongLong(), qint64(300));
}

void TestExtensionStats::noExtensionBucket()
{
    ExtensionStatsAggregator agg;
    agg.addFile(QStringLiteral("C:/README"), 42);
    const QVariantList list = agg.toVariantList();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list.first().toMap().value(QStringLiteral("extension")).toString(),
             QStringLiteral("(无扩展名)"));
}

void TestExtensionStats::clearRemovesStats()
{
    ExtensionStatsAggregator agg;
    agg.addFile(QStringLiteral("C:/x.dat"), 10);
    agg.clear();
    QCOMPARE(agg.toVariantList().size(), 0);
}

QTEST_MAIN(TestExtensionStats)
#include "test_extension_stats.moc"
