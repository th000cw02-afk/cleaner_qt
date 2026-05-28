#include "NtfsMftScanner_win.h"
#include "../models/FolderNode.h"
#include "../models/TopFilesTracker.h"
#include "../models/ExtensionStatsAggregator.h"
#include "../models/FileIndexBuilder.h"
#include "../models/FileIndexEntry.h"
#include "ScanFileRecorder.h"
#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QtConcurrent>
#include <atomic>
#include <functional>
#include <atomic>
#include <functional>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winioctl.h>
#endif

namespace {

#ifdef Q_OS_WIN
struct UsnEntry {
    quint64 fileReferenceNumber = 0;
    quint64 parentFileReferenceNumber = 0;
    QString fileName;
    bool isDirectory = false;
    qint64 size = 0;
};

bool isProcessElevated()
{
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }
    TOKEN_ELEVATION elevation{};
    DWORD size = 0;
    const BOOL ok = GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size);
    CloseHandle(token);
    return ok && elevation.TokenIsElevated;
}

HANDLE openVolumeHandle(wchar_t driveLetter)
{
    const QString volumePath = QStringLiteral("\\\\.\\%1:").arg(QChar(driveLetter));
    return CreateFileW(reinterpret_cast<LPCWSTR>(volumePath.utf16()),
                       GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       nullptr,
                       OPEN_EXISTING,
                       0,
                       nullptr);
}

bool queryFileSizeById(HANDLE volumeHandle, quint64 fileReferenceNumber, qint64& sizeOut)
{
    FILE_ID_DESCRIPTOR desc{};
    desc.dwSize = sizeof(desc);
    desc.Type = FileIdType;
    desc.FileId.QuadPart = static_cast<LONGLONG>(fileReferenceNumber);

    HANDLE fileHandle = OpenFileById(volumeHandle,
                                     &desc,
                                     FILE_READ_ATTRIBUTES,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                     nullptr,
                                     FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    FILE_STANDARD_INFO info{};
    const BOOL ok = GetFileInformationByHandleEx(fileHandle,
                                                 FileStandardInfo,
                                                 &info,
                                                 sizeof(info));
    CloseHandle(fileHandle);
    if (!ok) {
        return false;
    }
    sizeOut = static_cast<qint64>(info.EndOfFile.QuadPart);
    return true;
}

QString buildPath(const QHash<quint64, UsnEntry>& entries,
                  quint64 frn,
                  const QString& driveRoot,
                  QHash<quint64, QString>& cache)
{
    if (cache.contains(frn)) {
        return cache.value(frn);
    }
    const auto it = entries.constFind(frn);
    if (it == entries.constEnd()) {
        return QString();
    }
    const UsnEntry& entry = it.value();

    if (entry.fileReferenceNumber == 5 || entry.parentFileReferenceNumber == entry.fileReferenceNumber) {
        cache.insert(frn, driveRoot);
        return driveRoot;
    }

    QString parentPath = buildPath(entries, entry.parentFileReferenceNumber, driveRoot, cache);
    if (parentPath.isEmpty()) {
        parentPath = driveRoot;
    }
    QString path = parentPath;
    if (!path.endsWith(QLatin1Char('\\')) && !path.endsWith(QLatin1Char('/'))) {
        path += QLatin1Char('\\');
    }
    path += entry.fileName;
    cache.insert(frn, path);
    return path;
}

bool enumerateUsn(HANDLE volumeHandle,
                  QHash<quint64, UsnEntry>& entries,
                  const std::function<bool()>& shouldStop)
{
    constexpr DWORD bufferSize = 1024 * 1024;
    QByteArray buffer(bufferSize, Qt::Uninitialized);
    MFT_ENUM_DATA_V0 enumData{};
    enumData.StartFileReferenceNumber = 0;
    enumData.LowUsn = 0;
    enumData.HighUsn = MAXLONGLONG;

    while (!shouldStop()) {
        DWORD bytesReturned = 0;
        if (!DeviceIoControl(volumeHandle,
                             FSCTL_ENUM_USN_DATA,
                             &enumData,
                             sizeof(enumData),
                             buffer.data(),
                             bufferSize,
                             &bytesReturned,
                             nullptr)) {
            const DWORD err = GetLastError();
            if (err == ERROR_HANDLE_EOF) {
                return true;
            }
            return false;
        }
        if (bytesReturned <= sizeof(USN)) {
            break;
        }

        const USN nextUsn = *reinterpret_cast<const USN*>(buffer.constData());
        DWORD offset = sizeof(USN);
        while (offset < bytesReturned) {
            if (shouldStop()) {
                return false;
            }
            const auto* record = reinterpret_cast<const USN_RECORD*>(buffer.constData() + offset);
            if (record->RecordLength < sizeof(USN_RECORD)) {
                break;
            }

            const int nameLength = record->FileNameLength / static_cast<int>(sizeof(WCHAR));
            const WCHAR* namePtr = reinterpret_cast<const WCHAR*>(
                reinterpret_cast<const BYTE*>(record) + record->FileNameOffset);
            const QString fileName = QString::fromWCharArray(namePtr, nameLength);

            UsnEntry entry;
            entry.fileReferenceNumber = record->FileReferenceNumber;
            entry.parentFileReferenceNumber = record->ParentFileReferenceNumber;
            entry.fileName = fileName;
            entry.isDirectory = (record->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            entries.insert(entry.fileReferenceNumber, entry);
            offset += record->RecordLength;
        }
        enumData.StartFileReferenceNumber = nextUsn;
    }
    return false;
}
#endif

} // namespace

bool NtfsMftScanner::isNtfsVolume(const QString& rootPath)
{
    QStorageInfo storage(rootPath);
    return QString::fromUtf8(storage.fileSystemType())
        .compare(QStringLiteral("NTFS"), Qt::CaseInsensitive) == 0;
}

bool NtfsMftScanner::isElevated()
{
#ifdef Q_OS_WIN
    return isProcessElevated();
#else
    return false;
#endif
}

bool NtfsMftScanner::canUseMftScan(const QString& rootPath, QString& reason)
{
    if (!isNtfsVolume(rootPath)) {
        reason = QStringLiteral("非 NTFS 卷，使用目录扫描");
        return false;
    }
    if (!isElevated()) {
        reason = QStringLiteral("MFT 快速扫描需要管理员权限，将使用并行目录扫描");
        return false;
    }
    reason.clear();
    return true;
}

ScanResult NtfsMftScanner::scan(const ScanOptions& options,
                                StopCallback shouldStop,
                                ProgressCallback progressCallback)
{
    ScanResult result;
#ifdef Q_OS_WIN
    auto stopped = [&shouldStop]() {
        return shouldStop && shouldStop();
    };

    const QString rootNorm = QDir::fromNativeSeparators(
        QFileInfo(options.rootPath).absoluteFilePath());
    if (rootNorm.length() < 2) {
        return result;
    }

    const wchar_t driveLetter = rootNorm.at(0).toUpper().unicode();
    HANDLE volumeHandle = openVolumeHandle(driveLetter);
    if (volumeHandle == INVALID_HANDLE_VALUE) {
        return result;
    }

    QHash<quint64, UsnEntry> entries;
    if (!enumerateUsn(volumeHandle, entries, stopped)) {
        CloseHandle(volumeHandle);
        return result;
    }

    const QString driveRoot = QStringLiteral("%1:\\").arg(QChar(driveLetter));
    const QString rootNative = QDir::toNativeSeparators(rootNorm);
    QHash<quint64, QString> pathCache;

    FolderTreeBuilder builder(rootNorm);
    TopFilesTracker topFiles(options.topFileCount);
    FileIndexBuilder fileIndexBuilder;
    ExtensionStatsAggregator extStats;
    QMutex aggregateMutex;

    QList<quint64> fileFrns;
    fileFrns.reserve(entries.size());
    int dirCount = 0;
    for (auto it = entries.constBegin(); it != entries.constEnd(); ++it) {
        if (it->isDirectory) {
            dirCount++;
        } else {
            fileFrns.append(it.key());
        }
    }

    const int totalToProcess = fileFrns.size();
    std::atomic_int processed{0};
    qint64 totalSize = 0;
    int fileCount = 0;

    QtConcurrent::blockingMap(fileFrns, [&](quint64 frn) {
        if (stopped()) {
            return;
        }
        auto entryIt = entries.find(frn);
        if (entryIt == entries.end()) {
            return;
        }
        UsnEntry& entry = entryIt.value();
        qint64 size = 0;
        queryFileSizeById(volumeHandle, frn, size);
        entry.size = size;

        const QString nativePath = QDir::toNativeSeparators(
            buildPath(entries, frn, driveRoot, pathCache));
        const QString normPath = QDir::fromNativeSeparators(nativePath);
        if (!normPath.startsWith(rootNorm, Qt::CaseInsensitive)) {
            return;
        }
        if (options.shouldExcludePath(normPath)) {
            return;
        }
        if (entry.isDirectory || size < options.minFileSize) {
            return;
        }

        {
            QMutexLocker lock(&aggregateMutex);
            const ScanFileRecordResult recorded = recordScannedFile(
                normPath, size, QDateTime(), builder, topFiles, fileIndexBuilder, extStats,
                options.skipReparseAndCloud);
            if (recorded.skipped) {
                return;
            }
            totalSize += recorded.logicalForTotal;
            fileCount++;
        }

        const int done = ++processed;
        if (done % 5000 == 0 && progressCallback) {
            const int pct = totalToProcess > 0 ? qMin(99, done * 100 / totalToProcess) : -1;
            progressCallback(dirCount, totalSize, fileCount, pct);
        }
    });

    CloseHandle(volumeHandle);

    auto root = builder.root();
    if (root) {
        root->sortChildrenBySize();
    }

    result.root = root;
    result.topFiles = topFiles.sortedTopFiles();
    result.fileIndex = fileIndexBuilder.takeEntries();
    result.extensionStats = extStats.toVariantList();
    result.totalSize = totalSize;
    result.totalFiles = fileCount;
    result.directoriesScanned = dirCount;
    result.usedMft = true;
    result.scanPath = rootNorm;

    if (progressCallback) {
        progressCallback(dirCount, totalSize, fileCount, 100);
    }
#else
    Q_UNUSED(options);
    Q_UNUSED(shouldStop);
    Q_UNUSED(progressCallback);
#endif
    return result;
}
