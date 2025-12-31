#include "CleanerController.h"
#include <QDir>
#include <QFileInfoList>
#include <QStandardPaths>
#include <QDebug>
#include <QVariant>
#include <QVariantMap>
#include <algorithm>

CleanerController::CleanerController(QObject* parent)
    : QObject(parent)
    , m_scanner(new DiskScanner(this))
    , m_totalSize(0)
    , m_directoriesScanned(0)
{
    updateAvailableDrives();
    
    if (!m_availableDrives.isEmpty()) {
        m_currentDrive = m_availableDrives.first();
    }
    
    connect(m_scanner, &DiskScanner::fileFound, this, &CleanerController::onFileFound);
    connect(m_scanner, &DiskScanner::scanProgress, this, &CleanerController::onScanProgress);
    connect(m_scanner, &DiskScanner::scanFinished, this, &CleanerController::onScanFinished);
    connect(m_scanner, &DiskScanner::errorOccurred, this, &CleanerController::onScanError);
    connect(m_scanner, &DiskScanner::isScanningChanged, this, &CleanerController::isScanningChanged);
}

void CleanerController::updateAvailableDrives() {
    m_availableDrives.clear();
    
    // Windows 系统：获取所有驱动器
    QFileInfoList drives = QDir::drives();
    for (const QFileInfo& drive : drives) {
        QString drivePath = drive.absolutePath();
        QFileInfo info(drivePath);
        if (info.exists() && info.isReadable()) {
            m_availableDrives << drivePath;
        }
    }
    
    emit availableDrivesChanged();
}

void CleanerController::setCurrentDrive(const QString& drive) {
    if (m_currentDrive != drive) {
        m_currentDrive = drive;
        emit currentDriveChanged();
    }
}

void CleanerController::startScan() {
    if (m_currentDrive.isEmpty()) {
        emit scanError("No drive selected");
        return;
    }
    
    clearFileList();
    m_scanner->startScan(m_currentDrive);
}

void CleanerController::stopScan() {
    m_scanner->stopScan();
}

void CleanerController::deleteSelectedFiles(const QStringList& filePaths) {
    for (const QString& path : filePaths) {
        if (m_scanner->deleteFile(path)) {
            // 从列表中移除已删除的文件
            m_fileList.removeIf([&path](const FileInfo& info) {
                return info.path == path;
            });
        }
    }
    emit fileListUpdated();
    emit totalFilesChanged();
}

QVariantList CleanerController::getFileList() const {
    QVariantList list;
    for (const FileInfo& info : m_fileList) {
        QVariantMap map;
        map["path"] = info.path;
        map["size"] = info.size;
        map["lastModified"] = info.lastModified.toString(Qt::ISODate);
        map["isDirectory"] = info.isDirectory;
        list.append(map);
    }
    return list;
}

void CleanerController::clearFileList() {
    m_fileList.clear();
    m_totalSize = 0;
    m_directoriesScanned = 0;
    emit fileListUpdated();
    emit totalFilesChanged();
    emit totalSizeChanged();
    emit directoriesScannedChanged();
}

QString CleanerController::formatFileSize(qint64 bytes) const {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    const qint64 TB = GB * 1024;
    
    if (bytes >= TB) {
        return QString::number(bytes / double(TB), 'f', 2) + " TB";
    } else if (bytes >= GB) {
        return QString::number(bytes / double(GB), 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(bytes / double(MB), 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(bytes / double(KB), 'f', 2) + " KB";
    } else {
        return QString::number(bytes) + " B";
    }
}

void CleanerController::onFileFound(const FileInfo& fileInfo) {
    m_fileList.append(fileInfo);
    emit fileListUpdated();
    emit totalFilesChanged();
}

void CleanerController::onScanProgress(int directoriesScanned, qint64 totalSize, int fileCount) {
    m_directoriesScanned = directoriesScanned;
    m_totalSize = totalSize;
    emit directoriesScannedChanged();
    emit totalSizeChanged();
}

void CleanerController::onScanFinished() {
    // 按文件大小排序（从大到小）
    std::sort(m_fileList.begin(), m_fileList.end(), 
              [](const FileInfo& a, const FileInfo& b) {
                  return a.size > b.size;
              });
    emit fileListUpdated();
}

void CleanerController::onScanError(const QString& error) {
    emit scanError(error);
}

