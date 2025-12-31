#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QDateTime>

struct FileInfo {
    QString path;
    qint64 size;
    QDateTime lastModified;
    bool isDirectory;
    
    FileInfo() : size(0), isDirectory(false) {}
    
    FileInfo(const QString& p, qint64 s, const QDateTime& mod, bool isDir = false)
        : path(p), size(s), lastModified(mod), isDirectory(isDir) {}
};

#endif // FILEINFO_H

