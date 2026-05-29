#ifndef HARDLINKREGISTRY_H
#define HARDLINKREGISTRY_H

#include <QHash>
#include <QMutex>
#include <QString>

class HardlinkRegistry {
public:
    struct FileSizeInfo {
        qint64 logicalSize = 0;
        qint64 allocatedSize = 0;
    };

    FileSizeInfo registerFile(const QString& path, quint64 fileIndexKey, qint64 logicalSize, qint64 allocatedSize);
    void clear();
    int hardlinkGroupCount() const;

private:
    mutable QMutex m_mutex;
    QHash<quint64, int> m_refCount;
    QHash<quint64, qint64> m_logicalSize;
};

#endif // HARDLINKREGISTRY_H
