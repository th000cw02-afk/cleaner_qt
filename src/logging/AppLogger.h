#ifndef APPLOGGER_H
#define APPLOGGER_H

#include <QString>
#include <string>

class AppLogger {
public:
    enum class Level { Trace, Debug, Info, Warn, Error };

    static void init(const QString& appDir = QString(), bool logToFile = true);
    static void shutdown();
    static bool isInitialized();

    static QString logPath();
    static void setLevel(const QString& level);
    static void setLevelFromSettings();
    static void setLogToFile(bool enabled);

    static void log(Level level, const std::string& message);
    static void flush();

private:
    static void ensureLogger(const QString& appDir, bool logToFile);
    static Level levelFromString(const QString& level);
};

#define CQ_LOG_TRACE(msg) AppLogger::log(AppLogger::Level::Trace, (msg))
#define CQ_LOG_DEBUG(msg) AppLogger::log(AppLogger::Level::Debug, (msg))
#define CQ_LOG_INFO(msg) AppLogger::log(AppLogger::Level::Info, (msg))
#define CQ_LOG_WARN(msg) AppLogger::log(AppLogger::Level::Warn, (msg))
#define CQ_LOG_ERROR(msg) AppLogger::log(AppLogger::Level::Error, (msg))

void installQtMessageHandler();

#endif // APPLOGGER_H
