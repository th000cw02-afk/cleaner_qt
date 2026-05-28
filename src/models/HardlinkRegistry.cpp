#include "HardlinkRegistry.h"

HardlinkRegistry::FileSizeInfo HardlinkRegistry::registerFile(const QString& path, quint64 fileIndexKey,
                                            qint64 logicalSize, qint64 allocatedSize)
{
    Q_UNUSED(path);
    QMutexLocker lock(&m_mutex);
    FileSizeInfo info;
    info.allocatedSize = allocatedSize > 0 ? allocatedSize : logicalSize;

    if (fileIndexKey == 0) {
        info.logicalSize = logicalSize;
        return info;
    }

    const int refs = m_refCount.value(fileIndexKey, 0);
    m_refCount[fileIndexKey] = refs + 1;
    if (refs == 0) {
        m_logicalSize[fileIndexKey] = logicalSize;
        info.logicalSize = logicalSize;
    } else {
        info.logicalSize = 0;
    }
    return info;
}

void HardlinkRegistry::clear()
{
    QMutexLocker lock(&m_mutex);
    m_refCount.clear();
    m_logicalSize.clear();
}

int HardlinkRegistry::hardlinkGroupCount() const
{
    QMutexLocker lock(&m_mutex);
    int groups = 0;
    for (auto it = m_refCount.constBegin(); it != m_refCount.constEnd(); ++it) {
        if (it.value() > 1) {
            groups++;
        }
    }
    return groups;
}
