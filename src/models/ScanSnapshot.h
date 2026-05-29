#ifndef SCANSNAPSHOT_H
#define SCANSNAPSHOT_H

#include "../scan/ScanWorker.h"
#include <QString>

class ScanSnapshot {
public:
    static bool save(const QString& filePath, const ScanResult& result, QString* errorOut = nullptr);
    static bool load(const QString& filePath, ScanResult* resultOut, QString* errorOut = nullptr);
};

#endif // SCANSNAPSHOT_H
