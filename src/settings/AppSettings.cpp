#include "AppSettings.h"

#include "../logging/AppLogger.h"
#include "../scan/ScanOptions.h"

#include <QSettings>

#include <QCoreApplication>

#include <QDir>

#include <QFileInfo>



AppSettings& AppSettings::instance()

{

    static AppSettings settings;

    return settings;

}



AppSettings::AppSettings(QObject* parent)

    : QObject(parent)

{

    m_excludeDirNames = QStringList{

        QStringLiteral("$Recycle.Bin"),

        QStringLiteral("$RECYCLE.BIN"),

        QStringLiteral("System Volume Information"),

        QStringLiteral("$WinREAgent")

    };

    load();

}



QString AppSettings::settingsFilePath()

{

    const QString portableIni = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("CleanerQt.ini"));

    if (QFileInfo::exists(portableIni)) {

        return portableIni;

    }

    return QString();

}



void AppSettings::setLastScanPath(const QString& path)

{

    if (m_lastScanPath != path) {

        m_lastScanPath = path;

        emit lastScanPathChanged();

    }

}



void AppSettings::setMinFileSize(qint64 bytes)

{

    if (m_minFileSize != bytes) {

        m_minFileSize = bytes;

        emit minFileSizeChanged();

    }

}



void AppSettings::setExcludeDirNames(const QStringList& names)

{

    if (m_excludeDirNames != names) {

        m_excludeDirNames = names;

        emit excludeDirNamesChanged();

    }

}



void AppSettings::setStopAtMountPoints(bool value)

{

    if (m_stopAtMountPoints != value) {

        m_stopAtMountPoints = value;

        emit stopAtMountPointsChanged();

    }

}



void AppSettings::setPreferMftScan(bool value)

{

    if (m_preferMftScan != value) {

        m_preferMftScan = value;

        emit preferMftScanChanged();

    }

}



void AppSettings::setSearchFilter(const QString& filter)

{

    if (m_searchFilter != filter) {

        m_searchFilter = filter;

        emit searchFilterChanged();

    }

}



void AppSettings::setTheme(const QString& theme)

{

    const QString normalized = theme == QStringLiteral("dark") ? QStringLiteral("dark") : QStringLiteral("light");

    if (m_theme != normalized) {

        m_theme = normalized;

        emit themeChanged();

    }

}



void AppSettings::setPortableMode(bool value)

{

    if (m_portableMode != value) {

        m_portableMode = value;

        emit portableModeChanged();

    }

}



void AppSettings::setSkipReparseAndCloud(bool value)

{

    if (m_skipReparseAndCloud != value) {

        m_skipReparseAndCloud = value;

        emit skipReparseAndCloudChanged();

    }

}



void AppSettings::setCustomCleanupCommand(const QString& command)

{

    if (m_customCleanupCommand != command) {

        m_customCleanupCommand = command;

        emit customCleanupCommandChanged();

    }

}



void AppSettings::setCheckUpdates(bool value)

{

    if (m_checkUpdates != value) {

        m_checkUpdates = value;

        emit checkUpdatesChanged();

    }

}



void AppSettings::setLogLevel(const QString& level)

{

    const QString norm = level.trimmed().toLower();
    QString value = QStringLiteral("info");
    if (norm == QStringLiteral("trace") || norm == QStringLiteral("debug") || norm == QStringLiteral("info")
        || norm == QStringLiteral("warn") || norm == QStringLiteral("warning")
        || norm == QStringLiteral("error")) {
        value = norm == QStringLiteral("warning") ? QStringLiteral("warn") : norm;
    }
    if (m_logLevel != value) {
        m_logLevel = value;
        emit logLevelChanged();
        if (AppLogger::isInitialized()) {
            AppLogger::setLevel(m_logLevel);
        }
    }
}



void AppSettings::setLogToFile(bool value)

{

    if (m_logToFile != value) {

        m_logToFile = value;

        emit logToFileChanged();

        if (AppLogger::isInitialized()) {

            AppLogger::setLogToFile(m_logToFile);

        }

    }

}



void AppSettings::load()

{

    const QString portablePath = settingsFilePath();

    QSettings settings(portablePath.isEmpty() ? QStringLiteral("CleanerQt") : portablePath,

                       portablePath.isEmpty() ? QSettings::IniFormat : QSettings::IniFormat);

    if (!portablePath.isEmpty()) {

        settings.setPath(QSettings::IniFormat, QSettings::UserScope, portablePath);

    }

    m_lastScanPath = settings.value(QStringLiteral("lastScanPath")).toString();

    m_minFileSize = settings.value(QStringLiteral("minFileSize"), 0).toLongLong();

    m_excludeDirNames = settings.value(QStringLiteral("excludeDirNames"), m_excludeDirNames).toStringList();

    m_stopAtMountPoints = settings.value(QStringLiteral("stopAtMountPoints"), true).toBool();

    m_preferMftScan = settings.value(QStringLiteral("preferMftScan"), true).toBool();

    m_searchFilter = settings.value(QStringLiteral("searchFilter")).toString();

    m_theme = settings.value(QStringLiteral("theme"), QStringLiteral("light")).toString();

    m_portableMode = settings.value(QStringLiteral("portableMode"), !portablePath.isEmpty()).toBool();

    m_skipReparseAndCloud = settings.value(QStringLiteral("skipReparseAndCloud"), true).toBool();

    m_customCleanupCommand = settings.value(QStringLiteral("customCleanupCommand")).toString();

    m_checkUpdates = settings.value(QStringLiteral("checkUpdates"), true).toBool();

    m_logLevel = settings.value(QStringLiteral("logLevel"), QStringLiteral("info")).toString();

    m_logToFile = settings.value(QStringLiteral("logToFile"), true).toBool();

    if (AppLogger::isInitialized()) {

        AppLogger::setLogToFile(m_logToFile);

        AppLogger::setLevel(m_logLevel);

    }

}



void AppSettings::save()

{

    const QString portablePath = m_portableMode

        ? QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("CleanerQt.ini"))

        : settingsFilePath();

    QSettings settings(portablePath.isEmpty() ? QStringLiteral("CleanerQt") : portablePath,

                       QSettings::IniFormat);

    if (!portablePath.isEmpty()) {

        settings.setPath(QSettings::IniFormat, QSettings::UserScope, portablePath);

    }

    settings.setValue(QStringLiteral("lastScanPath"), m_lastScanPath);

    settings.setValue(QStringLiteral("minFileSize"), m_minFileSize);

    settings.setValue(QStringLiteral("excludeDirNames"), m_excludeDirNames);

    settings.setValue(QStringLiteral("stopAtMountPoints"), m_stopAtMountPoints);

    settings.setValue(QStringLiteral("preferMftScan"), m_preferMftScan);

    settings.setValue(QStringLiteral("searchFilter"), m_searchFilter);

    settings.setValue(QStringLiteral("theme"), m_theme);

    settings.setValue(QStringLiteral("portableMode"), m_portableMode);

    settings.setValue(QStringLiteral("skipReparseAndCloud"), m_skipReparseAndCloud);

    settings.setValue(QStringLiteral("customCleanupCommand"), m_customCleanupCommand);

    settings.setValue(QStringLiteral("checkUpdates"), m_checkUpdates);

    settings.setValue(QStringLiteral("logLevel"), m_logLevel);

    settings.setValue(QStringLiteral("logToFile"), m_logToFile);

}



void AppSettings::applyToScanOptions(ScanOptions& options) const

{

    options.minFileSize = m_minFileSize;

    options.excludeDirNames = m_excludeDirNames;

    options.stopAtMountPoints = m_stopAtMountPoints;

    options.skipReparseAndCloud = m_skipReparseAndCloud;

}

