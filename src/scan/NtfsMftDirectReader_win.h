#ifndef NTFSMFTDIRECTREADER_WIN_H
#define NTFSMFTDIRECTREADER_WIN_H

#include "../scan/ScanWorker.h"
#include "../scan/ScanOptions.h"
#include <functional>

class NtfsMftDirectReader {
public:
    using StopCallback = std::function<bool()>;
    using ProgressCallback = std::function<void(int, qint64, int, int)>;

    static bool isAvailable(const QString& rootPath, QString& reason);
    static ScanResult scan(const ScanOptions& options,
                           StopCallback shouldStop,
                           ProgressCallback progressCallback);
};

#endif // NTFSMFTDIRECTREADER_WIN_H
