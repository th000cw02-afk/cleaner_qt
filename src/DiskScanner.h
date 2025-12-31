#ifndef DISKSCANNER_H
#define DISKSCANNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QList>
#include "FileInfo.h"

class DiskScannerWorker : public QObject {
    Q_OBJECT
    
public:
    explicit DiskScannerWorker(QObject* parent = nullptr);
    
public slots:
    void scanDirectory(const QString& path);
    void stopScanning();
    
signals:
    void fileFound(const FileInfo& fileInfo);
    void scanProgress(int directoriesScanned, qint64 totalSize, int fileCount);
    void scanFinished();
    void errorOccurred(const QString& error);
    
private:
    bool m_stopRequested;
    qint64 m_totalSize;
    int m_fileCount;
    int m_directoryCount;
    
    void scanRecursive(const QString& path);
};

class DiskScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY isScanningChanged)
    
public:
    explicit DiskScanner(QObject* parent = nullptr);
    ~DiskScanner();
    
    bool isScanning() const { return m_isScanning; }
    
public slots:
    void startScan(const QString& path);
    void stopScan();
    bool deleteFile(const QString& filePath);
    bool deleteFiles(const QStringList& filePaths);
    
signals:
    void fileFound(const FileInfo& fileInfo);
    void scanProgress(int directoriesScanned, qint64 totalSize, int fileCount);
    void scanFinished();
    void isScanningChanged(bool isScanning);
    void errorOccurred(const QString& error);
    void fileDeleted(const QString& filePath, bool success);
    
private:
    QThread* m_workerThread;
    DiskScannerWorker* m_worker;
    bool m_isScanning;
    
    void setIsScanning(bool isScanning);
};

#endif // DISKSCANNER_H

