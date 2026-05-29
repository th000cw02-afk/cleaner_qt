#ifndef FILEINDEXENTRY_H
#define FILEINDEXENTRY_H

#include <QString>
#include <QDateTime>

struct FileIndexEntry {
    QString path;
    qint64 logicalSize = 0;
    qint64 allocatedSize = 0;
    QDateTime lastModified;
    QString extension;
    quint64 fileIndexKey = 0;

    FileIndexEntry() = default;
};

#endif // FILEINDEXENTRY_H
