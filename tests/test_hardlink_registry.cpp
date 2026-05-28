#include <QtTest>
#include "../src/models/HardlinkRegistry.h"

class TestHardlinkRegistry : public QObject {
    Q_OBJECT

private slots:
    void firstLinkCountsLogicalSize();
    void secondLinkReturnsZeroLogical();
    void fileIndexKeyZeroAlwaysCounts();
    void clearAndGroupCount();
};

void TestHardlinkRegistry::firstLinkCountsLogicalSize()
{
    HardlinkRegistry registry;
    const auto info = registry.registerFile(QStringLiteral("C:/a"), 42, 1000, 1000);
    QCOMPARE(info.logicalSize, qint64(1000));
    QCOMPARE(info.allocatedSize, qint64(1000));
}

void TestHardlinkRegistry::secondLinkReturnsZeroLogical()
{
    HardlinkRegistry registry;
    registry.registerFile(QStringLiteral("C:/a"), 42, 1000, 1000);
    const auto info = registry.registerFile(QStringLiteral("C:/b"), 42, 1000, 1000);
    QCOMPARE(info.logicalSize, qint64(0));
    QCOMPARE(registry.hardlinkGroupCount(), 1);
}

void TestHardlinkRegistry::fileIndexKeyZeroAlwaysCounts()
{
    HardlinkRegistry registry;
    const auto a = registry.registerFile(QStringLiteral("C:/a"), 0, 500, 500);
    const auto b = registry.registerFile(QStringLiteral("C:/b"), 0, 500, 500);
    QCOMPARE(a.logicalSize, qint64(500));
    QCOMPARE(b.logicalSize, qint64(500));
    QCOMPARE(registry.hardlinkGroupCount(), 0);
}

void TestHardlinkRegistry::clearAndGroupCount()
{
    HardlinkRegistry registry;
    registry.registerFile(QStringLiteral("C:/a"), 1, 100, 100);
    registry.registerFile(QStringLiteral("C:/b"), 1, 100, 100);
    registry.registerFile(QStringLiteral("C:/c"), 2, 200, 200);
    registry.registerFile(QStringLiteral("C:/d"), 2, 200, 200);
    QCOMPARE(registry.hardlinkGroupCount(), 2);
    registry.clear();
    QCOMPARE(registry.hardlinkGroupCount(), 0);
}

QTEST_MAIN(TestHardlinkRegistry)
#include "test_hardlink_registry.moc"
