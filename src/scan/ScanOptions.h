#ifndef SCANOPTIONS_H
#define SCANOPTIONS_H

#include <QString>
#include <QStringList>

struct ScanOptions {
    QString rootPath;
    QString volumeRoot;
    QStringList excludeDirNames;
    qint64 minFileSize = 0;
    int topFileCount = 100;
    bool stopAtMountPoints = true;
    bool skipReparseAndCloud = true;

    static ScanOptions defaultForPath(const QString& path);
    bool shouldExcludeDirName(const QString& dirName) const;
    bool shouldExcludePath(const QString& absolutePath) const;
    bool isOnScanVolume(const QString& absolutePath) const;
};

#endif // SCANOPTIONS_H
