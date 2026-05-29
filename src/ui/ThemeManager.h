#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isDark READ isDark NOTIFY themeChanged)
    Q_PROPERTY(QString windowBackground READ windowBackground NOTIFY themeChanged)
    Q_PROPERTY(QString panelBackground READ panelBackground NOTIFY themeChanged)
    Q_PROPERTY(QString textColor READ textColor NOTIFY themeChanged)
    Q_PROPERTY(QString secondaryTextColor READ secondaryTextColor NOTIFY themeChanged)
    Q_PROPERTY(QString accentColor READ accentColor NOTIFY themeChanged)
    Q_PROPERTY(QString borderColor READ borderColor NOTIFY themeChanged)
    Q_PROPERTY(QString hoverBackground READ hoverBackground NOTIFY themeChanged)
    Q_PROPERTY(QString selectionBackground READ selectionBackground NOTIFY themeChanged)
    Q_PROPERTY(QString zebraBackground READ zebraBackground NOTIFY themeChanged)
    Q_PROPERTY(QString trackBackground READ trackBackground NOTIFY themeChanged)
    Q_PROPERTY(QString successColor READ successColor NOTIFY themeChanged)
    Q_PROPERTY(QString warningColor READ warningColor NOTIFY themeChanged)
    Q_PROPERTY(QString errorColor READ errorColor NOTIFY themeChanged)
    Q_PROPERTY(int radius READ radius CONSTANT)
    Q_PROPERTY(int spacing READ spacing CONSTANT)
    Q_PROPERTY(int titleSize READ titleSize CONSTANT)
    Q_PROPERTY(int rowHeight READ rowHeight CONSTANT)

public:
    explicit ThemeManager(QObject* parent = nullptr);

    QString theme() const { return m_theme; }
    void setTheme(const QString& theme);
    bool isDark() const { return m_theme == QStringLiteral("dark"); }

    QString windowBackground() const;
    QString panelBackground() const;
    QString textColor() const;
    QString secondaryTextColor() const;
    QString accentColor() const;
    QString borderColor() const;
    QString hoverBackground() const;
    QString selectionBackground() const;
    QString zebraBackground() const;
    QString trackBackground() const;
    QString successColor() const;
    QString warningColor() const;
    QString errorColor() const;
    int radius() const { return 8; }
    int spacing() const { return 8; }
    int titleSize() const { return 15; }
    int rowHeight() const { return 40; }

signals:
    void themeChanged();

private:
    QString m_theme = QStringLiteral("light");
};

#endif // THEMEMANAGER_H
