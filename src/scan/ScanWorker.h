#ifndef SCANWORKER_H
#define SCANWORKER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <memory>
#include "ScanOptions.h"
#include "../models/FolderNode.h"
#include "../models/TopFilesTracker.h"
#include "../models/FileIndexBuilder.h"
#include "../models/FileIndexEntry.h"

class ExtensionStatsAggregator;

struct ScanResult {
    std::shared_ptr<FolderNode> root;
    QVector<FileInfo> topFiles;
    QVector<FileIndexEntry> fileIndex;
    QVariantList extensionStats;
    QVariantList duplicateGroups;
    qint64 totalSize = 0;
    qint64 totalAllocatedSize = 0;
    int totalFiles = 0;
    int directoriesScanned = 0;
    int skippedPaths = 0;
    bool usedMft = false;
    QString scanPath;
};

Q_DECLARE_METATYPE(ScanResult)
Q_DECLARE_METATYPE(ScanOptions)

class ScanWorker : public QObject {
    Q_OBJECT

public:
    explicit ScanWorker(QObject* parent = nullptr);

public slots:
    void scan(const ScanOptions& options);
    void stopScanning();

signals:
    void scanProgress(int directoriesScanned, qint64 totalSize, int fileCount, int progressPercent);
    void scanFinished(const ScanResult& result);
    void errorOccurred(const QString& error);
    void statusMessage(const QString& message);

private:
    bool m_stopRequested = false;
    int m_skippedPaths = 0;

    void scanDirectoryRecursive(const QString& path, const ScanOptions& options,
                                FolderTreeBuilder& builder, TopFilesTracker& topFiles,
                                FileIndexBuilder& fileIndexBuilder,
                                ExtensionStatsAggregator& extStats,
                                int& directoriesScanned, qint64& totalSize, qint64& totalAllocated,
                                int& totalFiles, qint64 volumeBytes);
    static constexpr int kProgressEmitFileInterval = 1000;
};

#endif // SCANWORKER_H
