#include "ScanWorker.h"

#include "../logging/AppLogger.h"
#include "../models/ExtensionStatsAggregator.h"

#include "ScanFileRecorder.h"

#include <QDir>

#include <QFileInfo>

#include <QStorageInfo>



ScanWorker::ScanWorker(QObject* parent)

    : QObject(parent)

{

}



void ScanWorker::stopScanning()

{

    m_stopRequested = true;

}



void ScanWorker::scan(const ScanOptions& options)

{

    m_stopRequested = false;

    m_skippedPaths = 0;

    QDir rootDir(options.rootPath);

    if (!rootDir.exists()) {

        const QString msg = QStringLiteral("目录不存在: %1").arg(options.rootPath);
        CQ_LOG_WARN(msg.toStdString());
        emit errorOccurred(msg);

        emit scanFinished(ScanResult{});

        return;

    }

    CQ_LOG_INFO(QStringLiteral("Scan started: %1").arg(options.rootPath).toStdString());



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



    scanDirectoryRecursive(rootPath, options, builder, topFiles, fileIndexBuilder, extStats,

                           directoriesScanned, totalSize, totalAllocated, totalFiles, volumeBytes);



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

    CQ_LOG_INFO(QStringLiteral("Scan finished: %1 files, %2 dirs, skipped %3")
                    .arg(totalFiles)
                    .arg(directoriesScanned)
                    .arg(m_skippedPaths)
                    .toStdString());

    emit scanFinished(result);

}



void ScanWorker::scanDirectoryRecursive(const QString& path, const ScanOptions& options,

                                        FolderTreeBuilder& builder, TopFilesTracker& topFiles,

                                        FileIndexBuilder& fileIndexBuilder,

                                        ExtensionStatsAggregator& extStats,

                                        int& directoriesScanned, qint64& totalSize, qint64& totalAllocated,

                                        int& totalFiles, qint64 volumeBytes)

{

    if (m_stopRequested) {

        return;

    }



    if (options.shouldExcludePath(path)) {

        m_skippedPaths++;

        return;

    }



    if (!options.isOnScanVolume(path)) {

        m_skippedPaths++;

        return;

    }



    QDir dir(path);

    if (!dir.exists()) {

        m_skippedPaths++;

        return;

    }



    builder.ensureFolder(path);

    directoriesScanned++;



    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);

    const QFileInfoList entries = dir.entryInfoList();



    for (const QFileInfo& entry : entries) {

        if (m_stopRequested) {

            return;

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

            scanDirectoryRecursive(entry.absoluteFilePath(), options, builder, topFiles,

                                   fileIndexBuilder, extStats, directoriesScanned, totalSize,

                                   totalAllocated, totalFiles, volumeBytes);

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

