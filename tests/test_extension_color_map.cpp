#include <QtTest>
#include "../src/models/ExtensionColorMap.h"

class TestExtensionColorMap : public QObject {
    Q_OBJECT

private slots:
    void normalizeExtensionWithSuffix();
    void normalizeExtensionNoSuffix();
    void colorStability();
};

void TestExtensionColorMap::normalizeExtensionWithSuffix()
{
    QCOMPARE(ExtensionColorMap::normalizeExtension(QStringLiteral("C:/dir/File.TXT")),
             QStringLiteral(".txt"));
}

void TestExtensionColorMap::normalizeExtensionNoSuffix()
{
    QCOMPARE(ExtensionColorMap::normalizeExtension(QStringLiteral("C:/dir/README")),
             QStringLiteral("(无扩展名)"));
}

void TestExtensionColorMap::colorStability()
{
    const QString c1 = ExtensionColorMap::colorForExtension(QStringLiteral(".txt"));
    const QString c2 = ExtensionColorMap::colorForExtension(QStringLiteral(".txt"));
    QCOMPARE(c1, c2);
    QVERIFY(c1.startsWith(QLatin1Char('#')));
}

QTEST_MAIN(TestExtensionColorMap)
#include "test_extension_color_map.moc"
