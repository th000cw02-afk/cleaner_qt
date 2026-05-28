#include "FileIndexBuilder.h"

void FileIndexBuilder::clear()
{
    QMutexLocker lock(&m_mutex);
    m_entries.clear();
    m_hardlinks.clear();
}

void FileIndexBuilder::addFile(const FileIndexEntry& entry)
{
    QMutexLocker lock(&m_mutex);
    m_entries.append(entry);
}

QVector<FileIndexEntry> FileIndexBuilder::entries() const
{
    QMutexLocker lock(&m_mutex);
    return m_entries;
}

QVector<FileIndexEntry> FileIndexBuilder::takeEntries()
{
    QMutexLocker lock(&m_mutex);
    return std::move(m_entries);
}

int FileIndexBuilder::count() const
{
    QMutexLocker lock(&m_mutex);
    return m_entries.size();
}
