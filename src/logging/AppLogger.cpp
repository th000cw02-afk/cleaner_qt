#include "AppLogger.h"

#include "../settings/AppSettings.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QTextStream>

#ifdef CLEANER_QT_HAS_SPDLOG
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#endif

namespace {

QMutex g_mutex;
QString g_logPath;
bool g_logToFile = true;
AppLogger::Level g_level = AppLogger::Level::Info;
bool g_initialized = false;

#ifdef CLEANER_QT_HAS_SPDLOG
std::shared_ptr<spdlog::logger> g_spdlog;
#endif

QString levelLabel(AppLogger::Level level)
{
    switch (level) {
    case AppLogger::Level::Trace:
        return QStringLiteral("trace");
    case AppLogger::Level::Debug:
        return QStringLiteral("debug");
    case AppLogger::Level::Info:
        return QStringLiteral("info");
    case AppLogger::Level::Warn:
        return QStringLiteral("warn");
    case AppLogger::Level::Error:
        return QStringLiteral("error");
    }
    return QStringLiteral("info");
}

bool shouldLog(AppLogger::Level level)
{
    return static_cast<int>(level) >= static_cast<int>(g_level);
}

void writeFallbackLine(AppLogger::Level level, const std::string& message)
{
    const QString line = QStringLiteral("[%1] [%2] %3")
                             .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")),
                                  levelLabel(level),
                                  QString::fromStdString(message));
    QTextStream(stdout) << line << '\n';

    if (g_logToFile && !g_logPath.isEmpty()) {
        QFile file(g_logPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << line << '\n';
        }
    }
}

#ifdef CLEANER_QT_HAS_SPDLOG
spdlog::level::level_enum toSpdLevel(AppLogger::Level level)
{
    switch (level) {
    case AppLogger::Level::Trace:
        return spdlog::level::trace;
    case AppLogger::Level::Debug:
        return spdlog::level::debug;
    case AppLogger::Level::Info:
        return spdlog::level::info;
    case AppLogger::Level::Warn:
        return spdlog::level::warn;
    case AppLogger::Level::Error:
        return spdlog::level::err;
    }
    return spdlog::level::info;
}

AppLogger::Level fromSpdString(const QString& level)
{
    const QString norm = level.trimmed().toLower();
    if (norm == QStringLiteral("trace")) {
        return AppLogger::Level::Trace;
    }
    if (norm == QStringLiteral("debug")) {
        return AppLogger::Level::Debug;
    }
    if (norm == QStringLiteral("warn") || norm == QStringLiteral("warning")) {
        return AppLogger::Level::Warn;
    }
    if (norm == QStringLiteral("error")) {
        return AppLogger::Level::Error;
    }
    return AppLogger::Level::Info;
}
#endif

} // namespace

AppLogger::Level AppLogger::levelFromString(const QString& level)
{
#ifdef CLEANER_QT_HAS_SPDLOG
    Q_UNUSED(level);
    return fromSpdString(level);
#else
    const QString norm = level.trimmed().toLower();
    if (norm == QStringLiteral("trace")) {
        return Level::Trace;
    }
    if (norm == QStringLiteral("debug")) {
        return Level::Debug;
    }
    if (norm == QStringLiteral("warn") || norm == QStringLiteral("warning")) {
        return Level::Warn;
    }
    if (norm == QStringLiteral("error")) {
        return Level::Error;
    }
    return Level::Info;
#endif
}

void AppLogger::ensureLogger(const QString& appDir, bool logToFile)
{
    QMutexLocker lock(&g_mutex);
    if (g_initialized) {
        return;
    }

    QString baseDir = appDir;
    if (baseDir.isEmpty() && QCoreApplication::instance()) {
        baseDir = QCoreApplication::applicationDirPath();
    }
    if (baseDir.isEmpty()) {
        baseDir = QDir::currentPath();
    }

    g_logToFile = logToFile;
    g_logPath.clear();
    if (logToFile) {
        const QString logsDir = QDir(baseDir).filePath(QStringLiteral("logs"));
        QDir().mkpath(logsDir);
        g_logPath = QDir(logsDir).filePath(QStringLiteral("CleanerQt.log"));
    }

#ifdef CLEANER_QT_HAS_SPDLOG
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    if (logToFile) {
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            g_logPath.toStdString(), 1024 * 1024 * 5, 3));
    }
    g_spdlog = std::make_shared<spdlog::logger>(QStringLiteral("CleanerQt").toStdString(),
                                                sinks.begin(), sinks.end());
    g_spdlog->set_level(spdlog::level::info);
    g_spdlog->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::register_logger(g_spdlog);
#endif

    g_initialized = true;
}

void AppLogger::init(const QString& appDir, bool logToFile)
{
    ensureLogger(appDir, logToFile);
    setLevelFromSettings();
}

void AppLogger::shutdown()
{
    QMutexLocker lock(&g_mutex);
#ifdef CLEANER_QT_HAS_SPDLOG
    if (g_spdlog) {
        g_spdlog->flush();
        spdlog::drop(g_spdlog->name());
        g_spdlog.reset();
    }
#endif
    g_initialized = false;
}

bool AppLogger::isInitialized()
{
    QMutexLocker lock(&g_mutex);
    return g_initialized;
}

QString AppLogger::logPath()
{
    QMutexLocker lock(&g_mutex);
    return g_logPath;
}

void AppLogger::setLevel(const QString& level)
{
    QMutexLocker lock(&g_mutex);
    g_level = levelFromString(level);
#ifdef CLEANER_QT_HAS_SPDLOG
    if (g_spdlog) {
        g_spdlog->set_level(toSpdLevel(g_level));
    }
#endif
}

void AppLogger::setLevelFromSettings()
{
    if (QCoreApplication::instance()) {
        setLevel(AppSettings::instance().logLevel());
    }
}

void AppLogger::setLogToFile(bool enabled)
{
    if (g_logToFile == enabled && g_initialized) {
        return;
    }
    const QString appDir = QCoreApplication::instance()
        ? QCoreApplication::applicationDirPath()
        : QString();
    shutdown();
    ensureLogger(appDir, enabled);
    setLevelFromSettings();
}

void AppLogger::log(Level level, const std::string& message)
{
    QMutexLocker lock(&g_mutex);
    if (!g_initialized || !shouldLog(level)) {
        return;
    }
#ifdef CLEANER_QT_HAS_SPDLOG
    if (g_spdlog) {
        g_spdlog->log(toSpdLevel(level), message);
        return;
    }
#endif
    writeFallbackLine(level, message);
}

void AppLogger::flush()
{
    QMutexLocker lock(&g_mutex);
#ifdef CLEANER_QT_HAS_SPDLOG
    if (g_spdlog) {
        g_spdlog->flush();
    }
#endif
}
