#include "ThemeManager.h"

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
{
}

void ThemeManager::setTheme(const QString& theme)
{
    const QString normalized = theme == QStringLiteral("dark") ? QStringLiteral("dark") : QStringLiteral("light");
    if (m_theme != normalized) {
        m_theme = normalized;
        emit themeChanged();
    }
}

QString ThemeManager::windowBackground() const
{
    return isDark() ? QStringLiteral("#121212") : QStringLiteral("#f5f5f5");
}

QString ThemeManager::panelBackground() const
{
    return isDark() ? QStringLiteral("#1e1e1e") : QStringLiteral("#ffffff");
}

QString ThemeManager::textColor() const
{
    return isDark() ? QStringLiteral("#eeeeee") : QStringLiteral("#212121");
}

QString ThemeManager::secondaryTextColor() const
{
    return isDark() ? QStringLiteral("#9e9e9e") : QStringLiteral("#616161");
}

QString ThemeManager::accentColor() const
{
    return isDark() ? QStringLiteral("#64b5f6") : QStringLiteral("#1976d2");
}

QString ThemeManager::borderColor() const
{
    return isDark() ? QStringLiteral("#3d3d3d") : QStringLiteral("#e0e0e0");
}

QString ThemeManager::hoverBackground() const
{
    return isDark() ? QStringLiteral("#2a2a2a") : QStringLiteral("#f5f5f5");
}

QString ThemeManager::selectionBackground() const
{
    return isDark() ? QStringLiteral("#1e3a5f") : QStringLiteral("#e3f2fd");
}

QString ThemeManager::zebraBackground() const
{
    return isDark() ? QStringLiteral("#252525") : QStringLiteral("#fafafa");
}

QString ThemeManager::trackBackground() const
{
    return isDark() ? QStringLiteral("#333333") : QStringLiteral("#eeeeee");
}

QString ThemeManager::successColor() const
{
    return isDark() ? QStringLiteral("#81c784") : QStringLiteral("#2e7d32");
}

QString ThemeManager::warningColor() const
{
    return isDark() ? QStringLiteral("#ffb74d") : QStringLiteral("#f57c00");
}

QString ThemeManager::errorColor() const
{
    return isDark() ? QStringLiteral("#e57373") : QStringLiteral("#c62828");
}
