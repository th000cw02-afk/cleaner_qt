#ifndef DISKSCANNER_H
#define DISKSCANNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QStringList>
#include "FileInfo.h"
#include "scan/ScanOptions.h"
#include "scan/ScanWorker.h"

class DiskScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY isScanningChanged)

public:
    explicit DiskScanner(QObject* parent = nullptr);
    ~DiskScanner();

    bool isScanning() const { return m_isScanning; }

public slots:
    void startScan(const QString& path, qint64 minFileSize = 0, bool preferMft = true);
    void stopScan();
    bool deleteFile(const QString& filePath);
    bool deleteFilesToRecycleBin(const QStringList& filePaths, QString* errorOut = nullptr);

signals:
    void scanProgress(int directoriesScanned, qint64 totalSize, int fileCount, int progressPercent);
    void scanFinished(const ScanResult& result);
    void isScanningChanged(bool isScanning);
    void errorOccurred(const QString& error);
    void statusMessage(const QString& message);
    void fileDeleted(const QString& filePath, bool success);

private:
    enum class ScanMode { Mft, Parallel, Sequential };

    QThread* m_workerThread = nullptr;
    QObject* m_worker = nullptr;
    bool m_isScanning = false;
    ScanMode m_pendingFallback = ScanMode::Parallel;
    ScanOptions m_pendingOptions;

    void setIsScanning(bool isScanning);
    void startWorkerScan(const ScanOptions& options, ScanMode mode);
    void connectWorkerSignals(QObject* worker);
};

#endif // DISKSCANNER_H
