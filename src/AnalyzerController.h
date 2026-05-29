#ifndef ANALYZERCONTROLLER_H
#define ANALYZERCONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QElapsedTimer>
#include <QHash>
#include <QTimer>
#include "DiskScanner.h"
#include "models/FolderTreeModel.h"
#include "models/FileIndexModel.h"
#include "models/FileIndexEntry.h"
#include "models/TreemapLayout.h"
#include "ui/ThemeManager.h"
#include "platform/FileWatcherService_win.h"

class AnalyzerController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList availableDrives READ availableDrives NOTIFY availableDrivesChanged)
    Q_PROPERTY(QString scanPath READ scanPath WRITE setScanPath NOTIFY scanPathChanged)
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY isScanningChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin CONSTANT)
    Q_PROPERTY(bool canUseMft READ canUseMft NOTIFY scanPathChanged)
    Q_PROPERTY(int scanProgress READ scanProgress NOTIFY scanProgressChanged)
    Q_PROPERTY(bool scanProgressIndeterminate READ scanProgressIndeterminate NOTIFY scanProgressChanged)
    Q_PROPERTY(int totalFiles READ totalFiles NOTIFY totalFilesChanged)
    Q_PROPERTY(qint64 totalSize READ totalSize NOTIFY totalSizeChanged)
    Q_PROPERTY(qint64 totalAllocatedSize READ totalAllocatedSize NOTIFY totalAllocatedSizeChanged)
    Q_PROPERTY(double scanRateBytesPerSec READ scanRateBytesPerSec NOTIFY scanRateChanged)
    Q_PROPERTY(int directoriesScanned READ directoriesScanned NOTIFY directoriesScannedChanged)
    Q_PROPERTY(int skippedPaths READ skippedPaths NOTIFY skippedPathsChanged)
    Q_PROPERTY(qint64 minFileSizeFilter READ minFileSizeFilter WRITE setMinFileSizeFilter NOTIFY minFileSizeFilterChanged)
    Q_PROPERTY(QString searchFilter READ searchFilter WRITE setSearchFilter NOTIFY searchFilterChanged)
    Q_PROPERTY(bool regexSearch READ regexSearch WRITE setRegexSearch NOTIFY regexSearchChanged)
    Q_PROPERTY(bool useAllocatedSize READ useAllocatedSize WRITE setUseAllocatedSize NOTIFY useAllocatedSizeChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(FolderTreeModel* folderTreeModel READ folderTreeModel CONSTANT)
    Q_PROPERTY(FileIndexModel* fileIndexModel READ fileIndexModel CONSTANT)
    Q_PROPERTY(TreemapLayout* treemapLayout READ treemapLayout CONSTANT)
    Q_PROPERTY(ThemeManager* themeManager READ themeManager CONSTANT)
    Q_PROPERTY(FileWatcherService* fileWatcher READ fileWatcher CONSTANT)
    Q_PROPERTY(QVariantList topFiles READ topFiles NOTIFY topFilesChanged)
    Q_PROPERTY(QVariantList filteredTopFiles READ filteredTopFiles NOTIFY filteredTopFilesChanged)
    Q_PROPERTY(QVariantList extensionStats READ extensionStats NOTIFY extensionStatsChanged)
    Q_PROPERTY(QVariantList topLevelFolders READ topLevelFolders NOTIFY topLevelFoldersChanged)
    Q_PROPERTY(QVariantList selectedFolderFiles READ selectedFolderFiles NOTIFY selectedFolderFilesChanged)
    Q_PROPERTY(QVariantList duplicateGroups READ duplicateGroups NOTIFY duplicateGroupsChanged)
    Q_PROPERTY(QStringList cleanupModuleIds READ cleanupModuleIds CONSTANT)
    Q_PROPERTY(QString selectedFolderPath READ selectedFolderPath NOTIFY selectedFolderPathChanged)
    Q_PROPERTY(QString treemapFocusPath READ treemapFocusPath WRITE setTreemapFocusPath NOTIFY treemapFocusPathChanged)
    Q_PROPERTY(QString extensionFilter READ extensionFilter WRITE setExtensionFilter NOTIFY extensionFilterChanged)
    Q_PROPERTY(qint64 scanElapsedMs READ scanElapsedMs NOTIFY scanElapsedMsChanged)
    Q_PROPERTY(bool usedMftScan READ usedMftScan NOTIFY usedMftScanChanged)
    Q_PROPERTY(QVariantList cleanupResults READ cleanupResults NOTIFY cleanupResultsChanged)
    Q_PROPERTY(int currentView READ currentView WRITE setCurrentView NOTIFY currentViewChanged)

public:
    explicit AnalyzerController(QObject* parent = nullptr);

    QStringList availableDrives() const { return m_availableDrives; }
    QString scanPath() const { return m_scanPath; }
    void setScanPath(const QString& path);

    bool isScanning() const;
    bool isAdmin() const;
    bool canUseMft() const;

    int scanProgress() const { return m_scanProgress; }
    bool scanProgressIndeterminate() const { return m_scanProgressIndeterminate; }
    int totalFiles() const { return m_totalFiles; }
    qint64 totalSize() const { return m_totalSize; }
    qint64 totalAllocatedSize() const { return m_totalAllocatedSize; }
    double scanRateBytesPerSec() const { return m_scanRateBytesPerSec; }
    int directoriesScanned() const { return m_directoriesScanned; }
    int skippedPaths() const { return m_skippedPaths; }
    qint64 minFileSizeFilter() const { return m_minFileSizeFilter; }
    void setMinFileSizeFilter(qint64 bytes);
    QString searchFilter() const { return m_searchFilter; }
    void setSearchFilter(const QString& filter);
    bool regexSearch() const { return m_regexSearch; }
    void setRegexSearch(bool enabled);
    bool useAllocatedSize() const { return m_useAllocatedSize; }
    void setUseAllocatedSize(bool value);
    QString statusMessage() const { return m_statusMessage; }
    QString lastError() const { return m_lastError; }
    FolderTreeModel* folderTreeModel() { return &m_folderTreeModel; }
    FileIndexModel* fileIndexModel() { return &m_fileIndexModel; }
    TreemapLayout* treemapLayout() { return &m_treemapLayout; }
    ThemeManager* themeManager() { return &m_themeManager; }
    FileWatcherService* fileWatcher() { return &m_fileWatcher; }
    QVariantList topFiles() const { return m_topFiles; }
    QVariantList filteredTopFiles() const { return m_filteredTopFiles; }
    QVariantList extensionStats() const { return m_extensionStats; }
    QVariantList topLevelFolders() const { return m_topLevelFolders; }
    QVariantList selectedFolderFiles() const { return m_selectedFolderFiles; }
    QVariantList duplicateGroups() const { return m_duplicateGroups; }
    QStringList cleanupModuleIds() const { return m_cleanupModuleIds; }
    QString selectedFolderPath() const { return m_selectedFolderPath; }
    QString treemapFocusPath() const { return m_treemapFocusPath; }
    void setTreemapFocusPath(const QString& path);
    QString extensionFilter() const { return m_extensionFilter; }
    void setExtensionFilter(const QString& ext);
    qint64 scanElapsedMs() const { return m_scanElapsedMs; }
    bool usedMftScan() const { return m_usedMftScan; }
    QVariantList cleanupResults() const { return m_cleanupResults; }
    int currentView() const { return m_currentView; }
    void setCurrentView(int view);

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE QString pickFolder();
    Q_INVOKABLE QString pickOpenFile();
    Q_INVOKABLE QString pickSaveFile(const QString& suggestedName);
    Q_INVOKABLE void deleteSelectedFiles(const QStringList& filePaths);
    Q_INVOKABLE void openPath(const QString& path);
    Q_INVOKABLE void revealInExplorer(const QString& path);
    Q_INVOKABLE void copyPathToClipboard(const QString& path);
    Q_INVOKABLE void selectFolderInTree(const QString& folderPath);
    Q_INVOKABLE void drillTreemap(const QString& folderPath);
    Q_INVOKABLE void treemapGoUp();
    Q_INVOKABLE bool exportCsv(const QString& filePath);
    Q_INVOKABLE bool exportHtml(const QString& filePath);
    Q_INVOKABLE bool saveSnapshot(const QString& filePath);
    Q_INVOKABLE bool loadSnapshot(const QString& filePath);
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void scanEmptyDirectories();
    Q_INVOKABLE void scanDuplicateCandidates();
    Q_INVOKABLE void runCleanupModule(const QString& moduleId);
    Q_INVOKABLE void restartAsAdmin();
    Q_INVOKABLE void runCustomCleanupOnSelection(const QStringList& paths);
    Q_INVOKABLE QString formatFileSize(qint64 bytes) const;
    Q_INVOKABLE void clearResults();

signals:
    void availableDrivesChanged();
    void scanPathChanged();
    void isScanningChanged();
    void scanProgressChanged();
    void totalFilesChanged();
    void totalSizeChanged();
    void totalAllocatedSizeChanged();
    void scanRateChanged();
    void directoriesScannedChanged();
    void skippedPathsChanged();
    void minFileSizeFilterChanged();
    void searchFilterChanged();
    void regexSearchChanged();
    void useAllocatedSizeChanged();
    void statusMessageChanged();
    void lastErrorChanged();
    void topFilesChanged();
    void filteredTopFilesChanged();
    void extensionStatsChanged();
    void topLevelFoldersChanged();
    void selectedFolderFilesChanged();
    void duplicateGroupsChanged();
    void selectedFolderPathChanged();
    void treemapFocusPathChanged();
    void extensionFilterChanged();
    void scanElapsedMsChanged();
    void usedMftScanChanged();
    void cleanupResultsChanged();
    void currentViewChanged();
    void scanError(const QString& error);
    void scanFinished();

private slots:
    void onScanProgress(int directoriesScanned, qint64 totalSize, int fileCount, int progressPercent);
    void flushThrottledScanProgress();
    void onScanFinished(const ScanResult& result);
    void applyPendingScanResult();
    void finalizeScanPresentation();
    void onScanError(const QString& error);
    void onStatusMessage(const QString& message);

private:
    void updateAvailableDrives();
    void setLastError(const QString& error);
    void setStatusMessage(const QString& message);
    void loadFilesForFolder(const QString& folderPath);
    void applyScanResult(const ScanResult& result, bool finalPass);
    void rebuildFilteredTopFiles();
    void rebuildTopLevelFolders();
    void updateTreemap();
    void syncFileIndexFilters();
    void rebuildFilesByParentCache();
    QVector<FileIndexEntry> directFilesForFocus() const;
    QVariantList fileInfoVectorToVariantList(const QVector<FileInfo>& files) const;
    QVariantList filterFileVariantList(const QVariantList& list) const;

    DiskScanner* m_scanner;
    FolderTreeModel m_folderTreeModel;
    FileIndexModel m_fileIndexModel;
    TreemapLayout m_treemapLayout;
    ThemeManager m_themeManager;
    FileWatcherService m_fileWatcher;
    QStringList m_availableDrives;
    QStringList m_cleanupModuleIds;
    QString m_scanPath;
    int m_scanProgress = 0;
    bool m_scanProgressIndeterminate = false;
    int m_totalFiles = 0;
    qint64 m_totalSize = 0;
    qint64 m_totalAllocatedSize = 0;
    double m_scanRateBytesPerSec = 0;
    int m_directoriesScanned = 0;
    int m_skippedPaths = 0;
    qint64 m_minFileSizeFilter = 0;
    QString m_searchFilter;
    bool m_regexSearch = false;
    bool m_useAllocatedSize = false;
    QString m_statusMessage;
    QString m_lastError;
    QVariantList m_topFiles;
    QVariantList m_filteredTopFiles;
    QVariantList m_extensionStats;
    QVariantList m_topLevelFolders;
    QVariantList m_selectedFolderFiles;
    QVariantList m_duplicateGroups;
    QString m_selectedFolderPath;
    QString m_treemapFocusPath;
    QString m_extensionFilter;
    qint64 m_scanElapsedMs = 0;
    bool m_usedMftScan = false;
    QVariantList m_cleanupResults;
    int m_currentView = 0;
    QElapsedTimer m_scanTimer;
    QElapsedTimer m_rateTimer;
    qint64 m_lastRateBytes = 0;
    QTimer m_progressThrottleTimer;
    int m_pendingDirectoriesScanned = 0;
    qint64 m_pendingTotalSize = 0;
    int m_pendingTotalFiles = 0;
    int m_pendingProgressPercent = -1;
    bool m_hasPendingProgress = false;
    ScanResult m_pendingScanResult;
    bool m_hasPendingScanResult = false;
    QHash<QString, QVector<FileIndexEntry>> m_filesByParent;
    std::shared_ptr<FolderNode> m_treeRoot;
    QVector<FileInfo> m_topFilesRaw;
    QVector<FileIndexEntry> m_fileIndex;
    ScanResult m_lastScanResult;
};

#endif // ANALYZERCONTROLLER_H
