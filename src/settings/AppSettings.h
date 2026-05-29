#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QStringList>

struct ScanOptions;

class AppSettings : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString lastScanPath READ lastScanPath WRITE setLastScanPath NOTIFY lastScanPathChanged)
    Q_PROPERTY(qint64 minFileSize READ minFileSize WRITE setMinFileSize NOTIFY minFileSizeChanged)
    Q_PROPERTY(QStringList excludeDirNames READ excludeDirNames WRITE setExcludeDirNames NOTIFY excludeDirNamesChanged)
    Q_PROPERTY(bool stopAtMountPoints READ stopAtMountPoints WRITE setStopAtMountPoints NOTIFY stopAtMountPointsChanged)
    Q_PROPERTY(bool preferMftScan READ preferMftScan WRITE setPreferMftScan NOTIFY preferMftScanChanged)
    Q_PROPERTY(QString searchFilter READ searchFilter WRITE setSearchFilter NOTIFY searchFilterChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool portableMode READ portableMode WRITE setPortableMode NOTIFY portableModeChanged)
    Q_PROPERTY(bool skipReparseAndCloud READ skipReparseAndCloud WRITE setSkipReparseAndCloud NOTIFY skipReparseAndCloudChanged)
    Q_PROPERTY(QString customCleanupCommand READ customCleanupCommand WRITE setCustomCleanupCommand NOTIFY customCleanupCommandChanged)
    Q_PROPERTY(bool checkUpdates READ checkUpdates WRITE setCheckUpdates NOTIFY checkUpdatesChanged)
    Q_PROPERTY(QString logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(bool logToFile READ logToFile WRITE setLogToFile NOTIFY logToFileChanged)

public:
    static AppSettings& instance();

    QString lastScanPath() const { return m_lastScanPath; }
    void setLastScanPath(const QString& path);

    qint64 minFileSize() const { return m_minFileSize; }
    void setMinFileSize(qint64 bytes);

    QStringList excludeDirNames() const { return m_excludeDirNames; }
    void setExcludeDirNames(const QStringList& names);

    bool stopAtMountPoints() const { return m_stopAtMountPoints; }
    void setStopAtMountPoints(bool value);

    bool preferMftScan() const { return m_preferMftScan; }
    void setPreferMftScan(bool value);

    QString searchFilter() const { return m_searchFilter; }
    void setSearchFilter(const QString& filter);

    QString theme() const { return m_theme; }
    void setTheme(const QString& theme);

    bool portableMode() const { return m_portableMode; }
    void setPortableMode(bool value);

    bool skipReparseAndCloud() const { return m_skipReparseAndCloud; }
    void setSkipReparseAndCloud(bool value);

    QString customCleanupCommand() const { return m_customCleanupCommand; }
    void setCustomCleanupCommand(const QString& command);

    bool checkUpdates() const { return m_checkUpdates; }
    void setCheckUpdates(bool value);

    QString logLevel() const { return m_logLevel; }
    void setLogLevel(const QString& level);

    bool logToFile() const { return m_logToFile; }
    void setLogToFile(bool value);

    static QString settingsFilePath();

    Q_INVOKABLE void load();
    Q_INVOKABLE void save();
    void applyToScanOptions(struct ScanOptions& options) const;

signals:
    void lastScanPathChanged();
    void minFileSizeChanged();
    void excludeDirNamesChanged();
    void stopAtMountPointsChanged();
    void preferMftScanChanged();
    void searchFilterChanged();
    void themeChanged();
    void portableModeChanged();
    void skipReparseAndCloudChanged();
    void customCleanupCommandChanged();
    void checkUpdatesChanged();
    void logLevelChanged();
    void logToFileChanged();

private:
    explicit AppSettings(QObject* parent = nullptr);

    QString m_lastScanPath;
    qint64 m_minFileSize = 0;
    QStringList m_excludeDirNames;
    bool m_stopAtMountPoints = true;
    bool m_preferMftScan = true;
    QString m_searchFilter;
    QString m_theme = QStringLiteral("light");
    bool m_portableMode = false;
    bool m_skipReparseAndCloud = true;
    QString m_customCleanupCommand;
    bool m_checkUpdates = true;
    QString m_logLevel = QStringLiteral("info");
    bool m_logToFile = true;
};

#endif // APPSETTINGS_H
