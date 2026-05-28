#include "DiskScanner.h"
#include "scan/ParallelScanWorker.h"
#include "scan/ScanWorker.h"
#include "scan/NtfsMftScanWorker.h"
#include "scan/NtfsMftScanner_win.h"
#include "platform/FileDeleter_win.h"
#include <QMetaType>

DiskScanner::DiskScanner(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<ScanResult>("ScanResult");
    qRegisterMetaType<ScanOptions>("ScanOptions");
}

DiskScanner::~DiskScanner()
{
    stopScan();
}

void DiskScanner::startScan(const QString& path, qint64 minFileSize, bool preferMft)
{
    if (m_isScanning) {
        stopScan();
    }

    ScanOptions options = ScanOptions::defaultForPath(path);
    options.minFileSize = minFileSize;
    m_pendingOptions = options;

    if (preferMft) {
        QString reason;
        if (NtfsMftScanner::canUseMftScan(options.rootPath, reason)) {
            if (!reason.isEmpty()) {
                emit statusMessage(reason);
            }
            startWorkerScan(options, ScanMode::Mft);
            return;
        }
        if (!reason.isEmpty()) {
            emit statusMessage(reason);
        }
    }

    startWorkerScan(options, ScanMode::Parallel);
}

void DiskScanner::connectWorkerSignals(QObject* worker)
{
    const auto progress = [this](int a, qint64 b, int c, int d) { emit scanProgress(a, b, c, d); };
    const auto error = [this](const QString& e) { emit errorOccurred(e); };
    const auto status = [this](const QString& s) { emit statusMessage(s); };

    if (auto* w = qobject_cast<ScanWorker*>(worker)) {
        connect(w, &ScanWorker::scanProgress, this, progress, Qt::QueuedConnection);
        connect(w, &ScanWorker::errorOccurred, this, error, Qt::QueuedConnection);
        connect(w, &ScanWorker::statusMessage, this, status, Qt::QueuedConnection);
    } else if (auto* w = qobject_cast<ParallelScanWorker*>(worker)) {
        connect(w, &ParallelScanWorker::scanProgress, this, progress, Qt::QueuedConnection);
        connect(w, &ParallelScanWorker::errorOccurred, this, error, Qt::QueuedConnection);
        connect(w, &ParallelScanWorker::statusMessage, this, status, Qt::QueuedConnection);
    } else if (auto* w = qobject_cast<NtfsMftScanWorker*>(worker)) {
        connect(w, &NtfsMftScanWorker::scanProgress, this, progress, Qt::QueuedConnection);
        connect(w, &NtfsMftScanWorker::errorOccurred, this, error, Qt::QueuedConnection);
        connect(w, &NtfsMftScanWorker::statusMessage, this, status, Qt::QueuedConnection);
    }
}

void DiskScanner::startWorkerScan(const ScanOptions& options, ScanMode mode)
{
    setIsScanning(true);
    m_workerThread = new QThread(this);

    if (mode == ScanMode::Mft) {
        auto* worker = new NtfsMftScanWorker();
        m_worker = worker;
        worker->moveToThread(m_workerThread);
        connect(m_workerThread, &QThread::started, worker, [worker, options]() {
            worker->scan(options);
        });
        connectWorkerSignals(worker);
        connect(worker, &NtfsMftScanWorker::scanFinished, this, [this, options](const ScanResult& result) {
            m_workerThread->quit();
            m_workerThread->wait(5000);
            m_worker = nullptr;
            if (result.usedMft && result.root) {
                setIsScanning(false);
                emit scanFinished(result);
                return;
            }
            emit statusMessage(QStringLiteral("MFT 扫描不可用，正在使用并行目录扫描…"));
            startWorkerScan(options, ScanMode::Parallel);
        }, Qt::QueuedConnection);
        connect(worker, &NtfsMftScanWorker::scanFinished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    } else if (mode == ScanMode::Parallel) {
        auto* worker = new ParallelScanWorker();
        m_worker = worker;
        worker->moveToThread(m_workerThread);
        connect(m_workerThread, &QThread::started, worker, [worker, options]() {
            worker->scan(options);
        });
        connectWorkerSignals(worker);
        connect(worker, &ParallelScanWorker::scanFinished, this, [this](const ScanResult& result) {
            setIsScanning(false);
            emit scanFinished(result);
            m_workerThread->quit();
        }, Qt::QueuedConnection);
        connect(worker, &ParallelScanWorker::scanFinished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    } else {
        auto* worker = new ScanWorker();
        m_worker = worker;
        worker->moveToThread(m_workerThread);
        connect(m_workerThread, &QThread::started, worker, [worker, options]() {
            worker->scan(options);
        });
        connectWorkerSignals(worker);
        connect(worker, &ScanWorker::scanFinished, this, [this](const ScanResult& result) {
            setIsScanning(false);
            emit scanFinished(result);
            m_workerThread->quit();
        }, Qt::QueuedConnection);
        connect(worker, &ScanWorker::scanFinished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    }

    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);
    m_workerThread->start();
}

void DiskScanner::stopScan()
{
    if (m_worker) {
        QMetaObject::invokeMethod(m_worker, "stopScanning", Qt::QueuedConnection);
    }
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait(5000);
    }
    m_worker = nullptr;
    setIsScanning(false);
}

bool DiskScanner::deleteFile(const QString& filePath)
{
    const bool success = FileDeleter::deletePermanently(filePath);
    emit fileDeleted(filePath, success);
    return success;
}

bool DiskScanner::deleteFilesToRecycleBin(const QStringList& filePaths, QString* errorOut)
{
    const bool success = FileDeleter::moveToRecycleBin(filePaths, errorOut);
    for (const QString& path : filePaths) {
        emit fileDeleted(path, success);
    }
    return success;
}

void DiskScanner::setIsScanning(bool isScanning)
{
    if (m_isScanning != isScanning) {
        m_isScanning = isScanning;
        emit isScanningChanged(m_isScanning);
    }
}
