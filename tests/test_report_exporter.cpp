#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include "../src/scan/ReportExporter.h"
#include "../src/models/FolderNode.h"
#include "../src/FileInfo.h"

class TestReportExporter : public QObject {
    Q_OBJECT

private slots:
    void exportCsvWithHeaderAndEscape();
    void exportHtmlContainsRoot();
};

void TestReportExporter::exportCsvWithHeaderAndEscape()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());

    auto root = std::make_shared<FolderNode>(QStringLiteral("C:/root"));
    root->size = 100;
    root->fileCount = 1;

    QVector<FileInfo> topFiles;
    topFiles.append(FileInfo(QStringLiteral("C:/root/a,\"big\".txt"), 100, QDateTime(), false));

    const QString csvPath = temp.filePath(QStringLiteral("out.csv"));
    QString error;
    QVERIFY(ReportExporter::exportCsv(csvPath, root, topFiles, &error));

    QFile file(csvPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());
    QVERIFY(content.contains(QStringLiteral("type,path,size_bytes,file_count")));
    QVERIFY(content.contains(QStringLiteral("folder,")));
    QVERIFY(content.contains(QStringLiteral("# top files")));
    QVERIFY(content.contains(QStringLiteral("\"\"big\"\"")));
}

void TestReportExporter::exportHtmlContainsRoot()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());

    auto root = std::make_shared<FolderNode>(QStringLiteral("D:/data"));
    root->size = 500;
    root->fileCount = 2;

    const QString htmlPath = temp.filePath(QStringLiteral("out.html"));
    QString error;
    QVERIFY(ReportExporter::exportHtml(htmlPath, root, {}, 500, &error));

    QFile file(htmlPath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());
    QVERIFY(content.contains(QStringLiteral("<!DOCTYPE html>")));
    QVERIFY(content.contains(QStringLiteral("D:/data")));
    QVERIFY(content.contains(QStringLiteral("500")));
}

QTEST_MAIN(TestReportExporter)
#include "test_report_exporter.moc"
