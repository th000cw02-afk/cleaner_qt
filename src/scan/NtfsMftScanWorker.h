#ifndef NTFSMFTSCANWORKER_H
#define NTFSMFTSCANWORKER_H

#include <QObject>
#include "ScanWorker.h"

class ScanOptions;

class NtfsMftScanWorker : public QObject {
    Q_OBJECT

public:
    explicit NtfsMftScanWorker(QObject* parent = nullptr);

public slots:
    void scan(const ScanOptions& options);
    void stopScanning();

signals:
    void scanProgress(int directoriesScanned, qint64 totalSize, int fileCount, int progressPercent);
    void scanFinished(const ScanResult& result);
    void errorOccurred(const QString& error);
    void statusMessage(const QString& message);

private:
    bool m_stopRequested = false;
};

#endif // NTFSMFTSCANWORKER_H
