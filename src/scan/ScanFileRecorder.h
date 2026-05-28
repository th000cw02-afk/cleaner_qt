#ifndef SCANFILERECORDER_H
#define SCANFILERECORDER_H

#include "../FileInfo.h"
#include "../models/ExtensionStatsAggregator.h"
#include "../models/FileIndexBuilder.h"
#include "../models/FileIndexEntry.h"
#include "../models/FolderNode.h"
#include "../models/TopFilesTracker.h"
#include "../models/ExtensionColorMap.h"
#include "../platform/FileIdHelper_win.h"
#include <QDateTime>
#include <QString>

struct ScanFileRecordResult {
    qint64 logicalForTotal = 0;
    qint64 allocatedForTotal = 0;
    bool skipped = false;
};

inline ScanFileRecordResult recordScannedFile(const QString& absolutePath,
                                              qint64 logicalSize,
                                              const QDateTime& modified,
                                              FolderTreeBuilder& builder,
                                              TopFilesTracker& topFiles,
                                              FileIndexBuilder& fileIndexBuilder,
                                              ExtensionStatsAggregator& extStats,
                                              bool skipReparseCloud)
{
    ScanFileRecordResult result;
    if (logicalSize < 0) {
        result.skipped = true;
        return result;
    }

    WindowsFileIds ids = FileIdHelper::queryFileIds(absolutePath, logicalSize);
    if (skipReparseCloud && FileIdHelper::shouldSkipReparseOrCloud(ids)) {
        result.skipped = true;
        return result;
    }

    const auto sizes = fileIndexBuilder.hardlinkRegistry()->registerFile(
        absolutePath, ids.fileIndex, logicalSize, ids.allocatedSize);

    builder.addFileSize(absolutePath, sizes.logicalSize, ids.allocatedSize, modified);
    extStats.addFile(absolutePath, sizes.logicalSize);
    topFiles.consider(FileInfo(absolutePath, sizes.logicalSize, modified, false));

    FileIndexEntry entry;
    entry.path = absolutePath;
    entry.logicalSize = logicalSize;
    entry.allocatedSize = ids.allocatedSize > 0 ? ids.allocatedSize : logicalSize;
    entry.lastModified = modified;
    entry.extension = ExtensionColorMap::normalizeExtension(absolutePath);
    entry.fileIndexKey = ids.fileIndex;
    fileIndexBuilder.addFile(entry);

    result.logicalForTotal = sizes.logicalSize;
    result.allocatedForTotal = entry.allocatedSize;
    return result;
}

#endif // SCANFILERECORDER_H
