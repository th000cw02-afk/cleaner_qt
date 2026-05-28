#ifndef NTFSMFTSCANNER_WIN_H
#define NTFSMFTSCANNER_WIN_H

#include <QString>
#include <functional>
#include "ScanWorker.h"

class NtfsMftScanner {
public:
    static bool isNtfsVolume(const QString& rootPath);
    static bool isElevated();
    static bool canUseMftScan(const QString& rootPath, QString& reason);

    using ProgressCallback = std::function<void(int directoriesScanned, qint64 totalSize, int fileCount, int progressPercent)>;
    using StopCallback = std::function<bool()>;

    static ScanResult scan(const ScanOptions& options,
                           StopCallback shouldStop,
                           ProgressCallback progressCallback);
};

#endif // NTFSMFTSCANNER_WIN_H
