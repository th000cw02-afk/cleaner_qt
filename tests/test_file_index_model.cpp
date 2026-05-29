#include <QtTest>
#include "../src/models/FileIndexModel.h"
#include "../src/models/FileIndexEntry.h"

class TestFileIndexModel : public QObject {
    Q_OBJECT

private slots:
    void textFilter();
    void extensionFilter();
    void regexFilter();
};

static QVector<FileIndexEntry> sampleEntries()
{
    QVector<FileIndexEntry> entries;
    FileIndexEntry a;
    a.path = QStringLiteral("C:/data/report.txt");
    a.logicalSize = 100;
    a.extension = QStringLiteral(".txt");
    entries.append(a);

    FileIndexEntry b;
    b.path = QStringLiteral("C:/data/image.png");
    b.logicalSize = 200;
    b.extension = QStringLiteral(".png");
    entries.append(b);

    FileIndexEntry c;
    c.path = QStringLiteral("D:/other/log.txt");
    c.logicalSize = 50;
    c.extension = QStringLiteral(".txt");
    entries.append(c);

    return entries;
}

void TestFileIndexModel::textFilter()
{
    FileIndexModel model;
    model.setEntries(sampleEntries());
    QCOMPARE(model.filteredRowCount(), 3);

    model.setFilterText(QStringLiteral("report"));
    QCOMPARE(model.filteredRowCount(), 1);
    QCOMPARE(model.pathAt(0), QStringLiteral("C:/data/report.txt"));
}

void TestFileIndexModel::extensionFilter()
{
    FileIndexModel model;
    model.setEntries(sampleEntries());
    model.setExtensionFilter(QStringLiteral(".png"));
    QCOMPARE(model.filteredRowCount(), 1);
    QCOMPARE(model.pathAt(0), QStringLiteral("C:/data/image.png"));
}

void TestFileIndexModel::regexFilter()
{
    FileIndexModel model;
    model.setEntries(sampleEntries());
    model.setRegexFilter(QStringLiteral("^D:/"), true);
    QCOMPARE(model.filteredRowCount(), 1);
    QCOMPARE(model.pathAt(0), QStringLiteral("D:/other/log.txt"));
}

QTEST_MAIN(TestFileIndexModel)
#include "test_file_index_model.moc"
