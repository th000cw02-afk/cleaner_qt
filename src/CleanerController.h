#ifndef CLEANERCONTROLLER_H
#define CLEANERCONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QVariantList>
#include "FileInfo.h"
#include "DiskScanner.h"

class CleanerController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList availableDrives READ availableDrives NOTIFY availableDrivesChanged)
    Q_PROPERTY(QString currentDrive READ currentDrive WRITE setCurrentDrive NOTIFY currentDriveChanged)
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY isScanningChanged)
    Q_PROPERTY(int totalFiles READ totalFiles NOTIFY totalFilesChanged)
    Q_PROPERTY(qint64 totalSize READ totalSize NOTIFY totalSizeChanged)
    Q_PROPERTY(int directoriesScanned READ directoriesScanned NOTIFY directoriesScannedChanged)
    
public:
    explicit CleanerController(QObject* parent = nullptr);
    
    QStringList availableDrives() const { return m_availableDrives; }
    QString currentDrive() const { return m_currentDrive; }
    void setCurrentDrive(const QString& drive);
    
    bool isScanning() const { return m_scanner ? m_scanner->isScanning() : false; }
    int totalFiles() const { return m_fileList.size(); }
    qint64 totalSize() const { return m_totalSize; }
    int directoriesScanned() const { return m_directoriesScanned; }
    
    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE void deleteSelectedFiles(const QStringList& filePaths);
    Q_INVOKABLE QVariantList getFileList() const;
    Q_INVOKABLE void clearFileList();
    Q_INVOKABLE QString formatFileSize(qint64 bytes) const;
    
signals:
    void availableDrivesChanged();
    void currentDriveChanged();
    void isScanningChanged();
    void totalFilesChanged();
    void totalSizeChanged();
    void directoriesScannedChanged();
    void fileListUpdated();
    void scanError(const QString& error);
    
private slots:
    void onFileFound(const FileInfo& fileInfo);
    void onScanProgress(int directoriesScanned, qint64 totalSize, int fileCount);
    void onScanFinished();
    void onScanError(const QString& error);
    
private:
    void updateAvailableDrives();
    
    DiskScanner* m_scanner;
    QStringList m_availableDrives;
    QString m_currentDrive;
    QList<FileInfo> m_fileList;
    qint64 m_totalSize;
    int m_directoriesScanned;
};

#endif // CLEANERCONTROLLER_H

