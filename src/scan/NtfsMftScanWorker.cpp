#include "NtfsMftScanWorker.h"
#include "NtfsMftScanner_win.h"

NtfsMftScanWorker::NtfsMftScanWorker(QObject* parent)
    : QObject(parent)
{
}

void NtfsMftScanWorker::stopScanning()
{
    m_stopRequested = true;
}

void NtfsMftScanWorker::scan(const ScanOptions& options)
{
    m_stopRequested = false;
    QString reason;
    if (!NtfsMftScanner::canUseMftScan(options.rootPath, reason)) {
        if (!reason.isEmpty()) {
            emit statusMessage(reason);
        }
        emit scanFinished(ScanResult{});
        return;
    }

    emit statusMessage(QStringLiteral("正在使用 MFT 快速扫描…"));

    ScanResult result = NtfsMftScanner::scan(
        options,
        [this]() { return m_stopRequested; },
        [this](int dirs, qint64 size, int files, int pct) {
            emit scanProgress(dirs, size, files, pct);
        });

    if (m_stopRequested) {
        emit scanFinished(ScanResult{});
        return;
    }

    if (!result.usedMft || !result.root) {
        emit statusMessage(QStringLiteral("MFT 扫描未完成，将回退目录扫描"));
        emit scanFinished(ScanResult{});
        return;
    }

    emit statusMessage(QStringLiteral("MFT 扫描完成"));
    emit scanFinished(result);
}
