#ifndef FILEINDEXBUILDER_H
#define FILEINDEXBUILDER_H

#include <QMutex>
#include <QVector>
#include "FileIndexEntry.h"
#include "HardlinkRegistry.h"

class FileIndexBuilder {
public:
    void clear();
    void addFile(const FileIndexEntry& entry);
    QVector<FileIndexEntry> entries() const;
    QVector<FileIndexEntry> takeEntries();
    int count() const;
    HardlinkRegistry* hardlinkRegistry() { return &m_hardlinks; }

private:
    mutable QMutex m_mutex;
    QVector<FileIndexEntry> m_entries;
    HardlinkRegistry m_hardlinks;
};

#endif // FILEINDEXBUILDER_H
