#include "NtfsMftDirectReader_win.h"
#include "NtfsMftScanner_win.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <winioctl.h>
#endif

bool NtfsMftDirectReader::isAvailable(const QString& rootPath, QString& reason)
{
#ifdef Q_OS_WIN
    if (!NtfsMftScanner::canUseMftScan(rootPath, reason)) {
        return false;
    }
    const QString volumePath = QStringLiteral("\\\\.\\%1:").arg(rootPath.at(0).toUpper());
    HANDLE volume = CreateFileW(reinterpret_cast<LPCWSTR>(volumePath.utf16()),
                                GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr,
                                OPEN_EXISTING,
                                0,
                                nullptr);
    if (volume == INVALID_HANDLE_VALUE) {
        reason = QStringLiteral("无法打开卷");
        return false;
    }
    NTFS_VOLUME_DATA_BUFFER data{};
    DWORD bytes = 0;
    const BOOL ok = DeviceIoControl(volume, FSCTL_GET_NTFS_VOLUME_DATA, nullptr, 0,
                                      &data, sizeof(data), &bytes, nullptr);
    CloseHandle(volume);
    if (!ok) {
        reason = QStringLiteral("无法读取 NTFS 卷数据");
        return false;
    }
    reason.clear();
    return true;
#else
    Q_UNUSED(rootPath);
    reason = QStringLiteral("仅 Windows 支持");
    return false;
#endif
}

ScanResult NtfsMftDirectReader::scan(const ScanOptions& options,
                                     StopCallback shouldStop,
                                     ProgressCallback progressCallback)
{
    QString reason;
    if (!isAvailable(options.rootPath, reason)) {
        return ScanResult{};
    }
    return NtfsMftScanner::scan(options, shouldStop, progressCallback);
}
