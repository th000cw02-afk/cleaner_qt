#ifndef CLEANUPSCANNERS_H
#define CLEANUPSCANNERS_H

#include <QString>
#include <QVector>
#include <QVariantList>

struct CleanupScanOptions {
    QString rootPath;
    qint64 minFileSize = 1024 * 1024;
    int maxResults = 500;
};

class CleanupScanners {
public:
    static QVariantList findEmptyDirectories(const CleanupScanOptions& options);
    static QVariantList findDuplicateCandidates(const CleanupScanOptions& options);
};

#endif // CLEANUPSCANNERS_H
