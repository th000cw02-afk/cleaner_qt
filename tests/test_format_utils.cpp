#include <QtTest>
#include "../src/util/FormatUtils.h"

class TestFormatUtils : public QObject {
    Q_OBJECT

private slots:
    void bytesAndKilobytes();
    void megabytesGigabytesTerabytes();
};

void TestFormatUtils::bytesAndKilobytes()
{
    QCOMPARE(FormatUtils::formatFileSize(0), QStringLiteral("0 B"));
    QCOMPARE(FormatUtils::formatFileSize(1023), QStringLiteral("1023 B"));
    QCOMPARE(FormatUtils::formatFileSize(1024), QStringLiteral("1.00 KB"));
}

void TestFormatUtils::megabytesGigabytesTerabytes()
{
    QCOMPARE(FormatUtils::formatFileSize(1024 * 1024), QStringLiteral("1.00 MB"));
    QCOMPARE(FormatUtils::formatFileSize(1024LL * 1024 * 1024), QStringLiteral("1.00 GB"));
    QCOMPARE(FormatUtils::formatFileSize(1024LL * 1024 * 1024 * 1024), QStringLiteral("1.00 TB"));
}

QTEST_MAIN(TestFormatUtils)
#include "test_format_utils.moc"
