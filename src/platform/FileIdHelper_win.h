#ifndef FILEIDHELPER_WIN_H
#define FILEIDHELPER_WIN_H

#include <QString>

struct WindowsFileIds {
    quint64 fileIndex = 0;
    qint64 allocatedSize = 0;
    bool isReparsePoint = false;
    bool isCloudPlaceholder = false;
};

class FileIdHelper {
public:
    static WindowsFileIds queryFileIds(const QString& path, qint64 logicalSize);
    static bool shouldSkipReparseOrCloud(const WindowsFileIds& ids);
};

#endif // FILEIDHELPER_WIN_H
