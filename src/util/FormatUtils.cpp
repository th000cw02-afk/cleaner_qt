#include "FormatUtils.h"

namespace FormatUtils {

QString formatFileSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    const qint64 TB = GB * 1024;

    if (bytes >= TB) {
        return QString::number(bytes / double(TB), 'f', 2) + QStringLiteral(" TB");
    }
    if (bytes >= GB) {
        return QString::number(bytes / double(GB), 'f', 2) + QStringLiteral(" GB");
    }
    if (bytes >= MB) {
        return QString::number(bytes / double(MB), 'f', 2) + QStringLiteral(" MB");
    }
    if (bytes >= KB) {
        return QString::number(bytes / double(KB), 'f', 2) + QStringLiteral(" KB");
    }
    return QString::number(bytes) + QStringLiteral(" B");
}

} // namespace FormatUtils
