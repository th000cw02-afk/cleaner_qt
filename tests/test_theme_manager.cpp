#include <QtTest>
#include <QSignalSpy>
#include "../src/ui/ThemeManager.h"

class TestThemeManager : public QObject {
    Q_OBJECT

private slots:
    void themeNormalization();
    void lightAndDarkPalettesDiffer();
    void layoutConstants();
    void themeChangedSignal();
};

void TestThemeManager::themeNormalization()
{
    ThemeManager theme;
    theme.setTheme(QStringLiteral("dark"));
    QCOMPARE(theme.theme(), QStringLiteral("dark"));
    QVERIFY(theme.isDark());
    theme.setTheme(QStringLiteral("neon"));
    QCOMPARE(theme.theme(), QStringLiteral("light"));
    QVERIFY(!theme.isDark());
}

void TestThemeManager::lightAndDarkPalettesDiffer()
{
    ThemeManager light;
    light.setTheme(QStringLiteral("light"));
    ThemeManager dark;
    dark.setTheme(QStringLiteral("dark"));
    QVERIFY(light.windowBackground() != dark.windowBackground());
    QVERIFY(light.textColor() != dark.textColor());
    QVERIFY(light.accentColor() != dark.accentColor());
}

void TestThemeManager::layoutConstants()
{
    ThemeManager theme;
    QCOMPARE(theme.radius(), 8);
    QCOMPARE(theme.spacing(), 8);
    QCOMPARE(theme.titleSize(), 15);
    QCOMPARE(theme.rowHeight(), 40);
}

void TestThemeManager::themeChangedSignal()
{
    ThemeManager theme;
    QSignalSpy spy(&theme, &ThemeManager::themeChanged);
    theme.setTheme(QStringLiteral("dark"));
    QCOMPARE(spy.count(), 1);
    theme.setTheme(QStringLiteral("dark"));
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestThemeManager)
#include "test_theme_manager.moc"
