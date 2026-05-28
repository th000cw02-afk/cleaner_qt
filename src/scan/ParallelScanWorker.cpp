#include "ParallelScanWorker.h"
#include "../models/ExtensionStatsAggregator.h"
#include "ScanFileRecorder.h"
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>
#include <QStorageInfo>
#include <QtConcurrent>

ParallelScanWorker::ParallelScanWorker(QObject* parent)
    : QObject(parent)
{
}

void ParallelScanWorker::stopScanning()
{
    m_stopRequested = true;
}

void ParallelScanWorker::scan(const ScanOptions& options)
{
    m_stopRequested = false;
    m_skippedPaths = 0;

    QDir rootDir(options.rootPath);
    if (!rootDir.exists()) {
        emit errorOccurred(QStringLiteral("目录不存在: %1").arg(options.rootPath));
        emit scanFinished(ScanResult{});
        return;
    }

    const QString rootPath = QDir::fromNativeSeparators(QFileInfo(options.rootPath).absoluteFilePath());
    FolderTreeBuilder builder(rootPath);
    TopFilesTracker topFiles(options.topFileCount);
    FileIndexBuilder fileIndexBuilder;
    ExtensionStatsAggregator extStats;

    int directoriesScanned = 0;
    qint64 totalSize = 0;
    qint64 totalAllocated = 0;
    int totalFiles = 0;
    QStorageInfo storage(rootPath);
    const qint64 volumeBytes = storage.bytesTotal() > 0 ? storage.bytesTotal() : 0;

    builder.ensureFolder(rootPath);
    directoriesScanned++;

    rootDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    const QFileInfoList entries = rootDir.entryInfoList();

    QVector<QString> subDirs;
    for (const QFileInfo& entry : entries) {
        if (m_stopRequested) {
            break;
        }
        if (entry.isDir()) {
            if (options.shouldExcludeDirName(entry.fileName())) {
                m_skippedPaths++;
                continue;
            }
            if (!options.isOnScanVolume(entry.absoluteFilePath())) {
                m_skippedPaths++;
                continue;
            }
            subDirs.append(entry.absoluteFilePath());
        } else if (entry.isFile()) {
            const qint64 size = entry.size();
            if (size < options.minFileSize) {
                continue;
            }
            const ScanFileRecordResult recorded = recordScannedFile(
                entry.absoluteFilePath(), size, entry.lastModified(), builder, topFiles,
                fileIndexBuilder, extStats, options.skipReparseAndCloud);
            if (recorded.skipped) {
                m_skippedPaths++;
                continue;
            }
            totalSize += recorded.logicalForTotal;
            totalAllocated += recorded.allocatedForTotal;
            totalFiles++;
        }
    }

    QtConcurrent::blockingMap(subDirs, [&](const QString& subPath) {
        if (m_stopRequested) {
            return;
        }
        scanDirectoryRecursive(subPath, options, builder, topFiles, fileIndexBuilder, extStats,
                               directoriesScanned, totalSize, totalAllocated, totalFiles, volumeBytes);
    });

    auto root = builder.root();
    if (root) {
        root->sortChildrenBySize();
    }

    ScanResult result;
    result.root = root;
    result.topFiles = topFiles.sortedTopFiles();
    result.fileIndex = fileIndexBuilder.takeEntries();
    result.extensionStats = extStats.toVariantList();
    result.totalSize = totalSize;
    result.totalAllocatedSize = totalAllocated;
    result.totalFiles = totalFiles;
    result.directoriesScanned = directoriesScanned;
    result.skippedPaths = m_skippedPaths;
    result.usedMft = false;
    result.scanPath = rootPath;

    emit scanProgress(directoriesScanned, totalSize, totalFiles, 100);
    emit scanFinished(result);
}

void ParallelScanWorker::scanDirectoryRecursive(const QString& path, const ScanOptions& options,
                                                FolderTreeBuilder& builder, TopFilesTracker& topFiles,
                                                FileIndexBuilder& fileIndexBuilder,
                                                ExtensionStatsAggregator& extStats,
                                                int& directoriesScanned, qint64& totalSize,
                                                qint64& totalAllocated, int& totalFiles,
                                                qint64 volumeBytes)
{
    if (m_stopRequested) {
        return;
    }

    if (options.shouldExcludePath(path) || !options.isOnScanVolume(path)) {
        QMutexLocker lock(&m_mutex);
        m_skippedPaths++;
        return;
    }

    QDir dir(path);
    if (!dir.exists()) {
        QMutexLocker lock(&m_mutex);
        m_skippedPaths++;
        return;
    }

    {
        QMutexLocker lock(&m_mutex);
        builder.ensureFolder(path);
        directoriesScanned++;
    }

    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    const QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        if (m_stopRequested) {
            return;
        }

        if (entry.isDir()) {
            if (options.shouldExcludeDirName(entry.fileName())) {
                QMutexLocker lock(&m_mutex);
                m_skippedPaths++;
                continue;
            }
            if (!options.isOnScanVolume(entry.absoluteFilePath())) {
                QMutexLocker lock(&m_mutex);
                m_skippedPaths++;
                continue;
            }
            scanDirectoryRecursive(entry.absoluteFilePath(), options, builder, topFiles,
                                   fileIndexBuilder, extStats, directoriesScanned, totalSize,
                                   totalAllocated, totalFiles, volumeBytes);
        } else if (entry.isFile()) {
            const qint64 size = entry.size();
            if (size < options.minFileSize) {
                continue;
            }

            ScanFileRecordResult recorded;
            {
                QMutexLocker lock(&m_mutex);
                recorded = recordScannedFile(entry.absoluteFilePath(), size, entry.lastModified(),
                                             builder, topFiles, fileIndexBuilder, extStats,
                                             options.skipReparseAndCloud);
                if (recorded.skipped) {
                    m_skippedPaths++;
                    continue;
                }
                totalSize += recorded.logicalForTotal;
                totalAllocated += recorded.allocatedForTotal;
                totalFiles++;

                if (totalFiles % kProgressEmitFileInterval == 0) {
                    int progress = -1;
                    if (volumeBytes > 0) {
                        progress = static_cast<int>(qMin<qint64>(99, (totalSize * 100) / volumeBytes));
                    }
                    emit scanProgress(directoriesScanned, totalSize, totalFiles, progress);
                }
            }
        }
    }
}
