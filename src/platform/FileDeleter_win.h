#ifndef FILEDELETER_WIN_H
#define FILEDELETER_WIN_H

#include <QStringList>

class FileDeleter {
public:
    static bool moveToRecycleBin(const QStringList& filePaths, QString* errorOut = nullptr);
    static bool deletePermanently(const QString& filePath);
};

#endif // FILEDELETER_WIN_H
