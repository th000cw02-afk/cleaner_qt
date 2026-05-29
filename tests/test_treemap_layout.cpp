#include <QtTest>
#include "../src/models/TreemapLayout.h"
#include "../src/models/FolderNode.h"
#include "../src/models/FileIndexEntry.h"

class TestTreemapLayout : public QObject {
    Q_OBJECT

private slots:
    void squarifyProducesRects();
    void hitTestFindsRect();
    void drillDownFocus();
    void directFilesStrip();
};

void TestTreemapLayout::squarifyProducesRects()
{
    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    auto a = std::make_shared<FolderNode>(QStringLiteral("C:/root/a"));
    a->size = 300;
    auto b = std::make_shared<FolderNode>(QStringLiteral("C:/root/b"));
    b->size = 700;
    root->addChild(a);
    root->addChild(b);
    root->size = 1000;

    TreemapLayout layout;
    layout.buildFromNode(root, 1000, root->path);
    QVERIFY(layout.rects().size() >= 2);
}

void TestTreemapLayout::hitTestFindsRect()
{
    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    auto a = std::make_shared<FolderNode>(QStringLiteral("C:/root/a"));
    a->size = 500;
    auto b = std::make_shared<FolderNode>(QStringLiteral("C:/root/b"));
    b->size = 500;
    root->addChild(a);
    root->addChild(b);
    root->size = 1000;

    TreemapLayout layout;
    layout.buildFromNode(root, 1000, root->path);
    QVERIFY(layout.rects().size() >= 2);

    const int hit = layout.hitTest(0.25, 0.5);
    QVERIFY(hit >= 0);
    QVERIFY(!layout.pathAt(hit).isEmpty());
}

void TestTreemapLayout::drillDownFocus()
{
    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    auto sub = std::make_shared<FolderNode>(QStringLiteral("C:/root/sub"));
    auto leaf = std::make_shared<FolderNode>(QStringLiteral("C:/root/sub/leaf"));
    leaf->size = 800;
    sub->addChild(leaf);
    sub->size = 800;
    root->addChild(sub);
    root->size = 800;

    TreemapLayout layout;
    layout.buildFromNode(root, 800, sub->path);
    QVERIFY(layout.rects().size() >= 1);
    QCOMPARE(layout.focusPath(), sub->path);
}

void TestTreemapLayout::directFilesStrip()
{
    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    root->size = 1000;

    QVector<FileIndexEntry> files;
    FileIndexEntry big;
    big.path = QStringLiteral("C:/root/big.dat");
    big.logicalSize = 600;
    big.extension = QStringLiteral(".dat");
    files.append(big);

    FileIndexEntry small;
    small.path = QStringLiteral("C:/root/small.dat");
    small.logicalSize = 400;
    files.append(small);

    FileIndexEntry zero;
    zero.path = QStringLiteral("C:/root/zero.dat");
    zero.logicalSize = 0;
    files.append(zero);

    FileIndexEntry other;
    other.path = QStringLiteral("C:/other/file.dat");
    other.logicalSize = 9000;
    files.append(other);

    TreemapLayout layout;
    layout.buildFromNode(root, 1000, root->path, files);
    QVERIFY(layout.rects().size() >= 2);

    bool foundBig = false;
    for (const QVariant& v : layout.rects()) {
        if (v.toMap().value(QStringLiteral("path")).toString().contains(QStringLiteral("big.dat"))) {
            foundBig = true;
            break;
        }
    }
    QVERIFY(foundBig);

    const int hit = layout.hitTest(0.9, 0.5);
    if (hit >= 0) {
        QVERIFY(layout.pathAt(hit).contains(QStringLiteral(".dat")));
    }
}

QTEST_MAIN(TestTreemapLayout)
#include "test_treemap_layout.moc"
