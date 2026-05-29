#include "FileIdHelper_win.h"
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

WindowsFileIds FileIdHelper::queryFileIds(const QString& path, qint64 logicalSize)
{
    WindowsFileIds ids;
    ids.allocatedSize = logicalSize;
#ifdef Q_OS_WIN
    const QString native = path;
    HANDLE handle = CreateFileW(reinterpret_cast<LPCWSTR>(native.utf16()),
                                FILE_READ_ATTRIBUTES,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                nullptr,
                                OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
                                nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        return ids;
    }

    BY_HANDLE_FILE_INFORMATION info{};
    if (GetFileInformationByHandle(handle, &info)) {
        ids.fileIndex = (static_cast<quint64>(info.nFileIndexHigh) << 32) | info.nFileIndexLow;
        ids.isReparsePoint = (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
        ids.isCloudPlaceholder = (info.dwFileAttributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) != 0
            || (info.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) != 0;
        const qint64 alloc = (static_cast<qint64>(info.nFileSizeHigh) << 32) | info.nFileSizeLow;
        if (alloc > 0) {
            ids.allocatedSize = alloc;
        }
    }
    CloseHandle(handle);
#else
    Q_UNUSED(path);
    Q_UNUSED(logicalSize);
#endif
    return ids;
}

bool FileIdHelper::shouldSkipReparseOrCloud(const WindowsFileIds& ids)
{
    return ids.isReparsePoint || ids.isCloudPlaceholder;
}
