#include "FileDeleter_win.h"
#include <QFile>
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

bool FileDeleter::moveToRecycleBin(const QStringList& filePaths, QString* errorOut)
{
#ifdef Q_OS_WIN
    if (filePaths.isEmpty()) {
        return true;
    }

    QString doubleNullSeparated;
    for (const QString& path : filePaths) {
        QString native = QDir::toNativeSeparators(path);
        doubleNullSeparated += native;
        doubleNullSeparated.append(QChar('\0'));
    }
    doubleNullSeparated.append(QChar('\0'));

    std::wstring buffer = doubleNullSeparated.toStdWString();

    SHFILEOPSTRUCTW op{};
    op.wFunc = FO_DELETE;
    op.pFrom = buffer.data();
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

    const int result = SHFileOperationW(&op);
    if (result != 0) {
        if (errorOut) {
            *errorOut = QStringLiteral("无法移至回收站 (错误码 %1)").arg(result);
        }
        return false;
    }
    return true;
#else
    Q_UNUSED(filePaths);
    if (errorOut) {
        *errorOut = QStringLiteral("回收站仅支持 Windows");
    }
    return false;
#endif
}

bool FileDeleter::deletePermanently(const QString& filePath)
{
    return QFile::remove(filePath);
}
