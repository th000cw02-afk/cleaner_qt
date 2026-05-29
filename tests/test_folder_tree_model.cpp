#include <QtTest>
#include "../src/models/FolderTreeModel.h"
#include "../src/models/FolderNode.h"

class TestFolderTreeModel : public QObject {
    Q_OBJECT

private slots:
    void indexParentAndSizeRole();
};

void TestFolderTreeModel::indexParentAndSizeRole()
{
    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    root->size = 1000;
    auto childA = std::make_shared<FolderNode>(QStringLiteral("C:/root/a"));
    childA->size = 600;
    auto childB = std::make_shared<FolderNode>(QStringLiteral("C:/root/b"));
    childB->size = 400;
    root->addChild(childA);
    root->addChild(childB);

    FolderTreeModel model;
    model.setTree(root, 1000);
    QCOMPARE(model.rootSize(), qint64(1000));
    QCOMPARE(model.rowCount(), 2);

    const QModelIndex childIndex = model.index(0, 0);
    QVERIFY(childIndex.isValid());
    QCOMPARE(model.data(childIndex, FolderTreeModel::SizeRole).toLongLong(), qint64(600));
    QCOMPARE(model.data(childIndex, FolderTreeModel::PathRole).toString(), QStringLiteral("C:/root/a"));

    const QModelIndex parentIndex = model.parent(childIndex);
    QVERIFY(!parentIndex.isValid());
}

QTEST_MAIN(TestFolderTreeModel)
#include "test_folder_tree_model.moc"
