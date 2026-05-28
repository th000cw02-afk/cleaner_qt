#include "ScanOptions.h"
#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>

ScanOptions ScanOptions::defaultForPath(const QString& path)
{
    ScanOptions opts;
    QFileInfo info(path);
    opts.rootPath = QDir::fromNativeSeparators(info.absoluteFilePath());
    QStorageInfo storage(opts.rootPath);
    opts.volumeRoot = QDir::fromNativeSeparators(storage.rootPath());
    if (opts.volumeRoot.isEmpty() && opts.rootPath.length() >= 2) {
        opts.volumeRoot = opts.rootPath.left(2) + QLatin1Char('/');
    }
    opts.excludeDirNames = QStringList{
        QStringLiteral("$Recycle.Bin"),
        QStringLiteral("$RECYCLE.BIN"),
        QStringLiteral("System Volume Information"),
        QStringLiteral("$WinREAgent")
    };
    return opts;
}

bool ScanOptions::shouldExcludeDirName(const QString& dirName) const
{
    return excludeDirNames.contains(dirName, Qt::CaseInsensitive);
}

bool ScanOptions::shouldExcludePath(const QString& absolutePath) const
{
    const QString norm = QDir::fromNativeSeparators(absolutePath);
    for (const QString& name : excludeDirNames) {
        if (norm.contains(QLatin1Char('/') + name, Qt::CaseInsensitive)
            || norm.endsWith(QLatin1Char('/') + name, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

bool ScanOptions::isOnScanVolume(const QString& absolutePath) const
{
    if (!stopAtMountPoints || volumeRoot.isEmpty()) {
        return true;
    }
    const QString norm = QDir::fromNativeSeparators(absolutePath);
    const QString vol = QDir::fromNativeSeparators(volumeRoot);
    return norm.startsWith(vol, Qt::CaseInsensitive);
}
