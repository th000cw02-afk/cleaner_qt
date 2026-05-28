#include "FileWatcherService_win.h"

FileWatcherService::FileWatcherService(QObject* parent)
    : QObject(parent)
{
}

FileWatcherService::~FileWatcherService() = default;

void FileWatcherService::startWatch(const QString& path)
{
    m_watchPath = path;
    m_watching = true;
    m_log.prepend(QStringLiteral("[watch] %1").arg(path));
    emit watchPathChanged();
    emit watchingChanged();
    emit logChanged();
}

void FileWatcherService::stopWatch()
{
    m_watching = false;
    m_watchPath.clear();
    emit watchPathChanged();
    emit watchingChanged();
}

void FileWatcherService::clearLog()
{
    m_log.clear();
    emit logChanged();
}
