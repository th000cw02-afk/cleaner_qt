#include "DiskScanner.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QFile>
#include <QDirIterator>

// DiskScannerWorker 实现
DiskScannerWorker::DiskScannerWorker(QObject* parent)
    : QObject(parent)
    , m_stopRequested(false)
    , m_totalSize(0)
    , m_fileCount(0)
    , m_directoryCount(0)
{
}

void DiskScannerWorker::scanDirectory(const QString& path) {
    m_stopRequested = false;
    m_totalSize = 0;
    m_fileCount = 0;
    m_directoryCount = 0;
    
    QDir dir(path);
    if (!dir.exists()) {
        emit errorOccurred(QString("Directory does not exist: %1").arg(path));
        emit scanFinished();
        return;
    }
    
    scanRecursive(path);
    emit scanFinished();
}

void DiskScannerWorker::stopScanning() {
    m_stopRequested = true;
}

void DiskScannerWorker::scanRecursive(const QString& path) {
    if (m_stopRequested) {
        return;
    }
    
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    
    m_directoryCount++;
    
    // 扫描文件
    QDirIterator it(path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext() && !m_stopRequested) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        
        if (fileInfo.exists() && fileInfo.isFile()) {
            qint64 size = fileInfo.size();
            m_totalSize += size;
            m_fileCount++;
            
            FileInfo info;
            info.path = filePath;
            info.size = size;
            info.lastModified = fileInfo.lastModified();
            info.isDirectory = false;
            
            emit fileFound(info);
            
            // 每扫描一定数量的文件后发送进度更新
            if (m_fileCount % 100 == 0) {
                emit scanProgress(m_directoryCount, m_totalSize, m_fileCount);
            }
        }
    }
    
    // 发送最终进度
    if (!m_stopRequested) {
        emit scanProgress(m_directoryCount, m_totalSize, m_fileCount);
    }
}

// DiskScanner 实现
DiskScanner::DiskScanner(QObject* parent)
    : QObject(parent)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
    , m_isScanning(false)
{
}

DiskScanner::~DiskScanner() {
    stopScan();
}

void DiskScanner::startScan(const QString& path) {
    if (m_isScanning) {
        stopScan();
    }
    
    setIsScanning(true);
    
    m_workerThread = new QThread(this);
    m_worker = new DiskScannerWorker();
    
    m_worker->moveToThread(m_workerThread);
    
    connect(m_workerThread, &QThread::started, [this, path]() {
        m_worker->scanDirectory(path);
    });
    
    connect(m_worker, &DiskScannerWorker::fileFound, this, &DiskScanner::fileFound);
    connect(m_worker, &DiskScannerWorker::scanProgress, this, &DiskScanner::scanProgress);
    connect(m_worker, &DiskScannerWorker::scanFinished, this, [this]() {
        setIsScanning(false);
        emit scanFinished();
        m_workerThread->quit();
    });
    connect(m_worker, &DiskScannerWorker::errorOccurred, this, &DiskScanner::errorOccurred);
    
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);
    
    m_workerThread->start();
}

void DiskScanner::stopScan() {
    if (m_worker && m_isScanning) {
        m_worker->stopScanning();
        if (m_workerThread && m_workerThread->isRunning()) {
            m_workerThread->quit();
            m_workerThread->wait(3000);
        }
    }
    setIsScanning(false);
}

bool DiskScanner::deleteFile(const QString& filePath) {
    QFile file(filePath);
    bool success = file.remove();
    emit fileDeleted(filePath, success);
    return success;
}

bool DiskScanner::deleteFiles(const QStringList& filePaths) {
    bool allSuccess = true;
    for (const QString& path : filePaths) {
        if (!deleteFile(path)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

void DiskScanner::setIsScanning(bool isScanning) {
    if (m_isScanning != isScanning) {
        m_isScanning = isScanning;
        emit isScanningChanged(m_isScanning);
    }
}

