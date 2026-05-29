#include "AnalyzerController.h"

#include "scan/ReportExporter.h"

#include "scan/NtfsMftScanner_win.h"

#include "scan/CleanupScanners.h"

#include "scan/ScanOptions.h"

#include "scan/modules/ScanModule.h"

#include "models/ScanSnapshot.h"

#include "settings/AppSettings.h"

#include "util/FormatUtils.h"

#include "platform/UacHelper_win.h"

#include <QDir>

#include <QFileInfo>

#include <QFileDialog>

#include <QDesktopServices>

#include <QGuiApplication>

#include <QClipboard>

#include <QUrl>

#include <QVariantMap>

#include <QProcess>



AnalyzerController::AnalyzerController(QObject* parent)

    : QObject(parent)

    , m_scanner(new DiskScanner(this))

{

    AppSettings::instance().load();

    m_minFileSizeFilter = AppSettings::instance().minFileSize();

    m_searchFilter = AppSettings::instance().searchFilter();

    m_themeManager.setTheme(AppSettings::instance().theme());

    m_cleanupModuleIds = ScanModuleRegistry::instance().moduleIds();



    updateAvailableDrives();

    if (!AppSettings::instance().lastScanPath().isEmpty()) {

        m_scanPath = AppSettings::instance().lastScanPath();

    } else if (!m_availableDrives.isEmpty()) {

        m_scanPath = m_availableDrives.first();

    }



    connect(m_scanner, &DiskScanner::scanProgress, this, &AnalyzerController::onScanProgress,
            Qt::QueuedConnection);

    connect(m_scanner, &DiskScanner::scanFinished, this, &AnalyzerController::onScanFinished,
            Qt::QueuedConnection);

    m_progressThrottleTimer.setSingleShot(true);
    m_progressThrottleTimer.setInterval(120);
    connect(&m_progressThrottleTimer, &QTimer::timeout, this,
            &AnalyzerController::flushThrottledScanProgress);

    connect(m_scanner, &DiskScanner::errorOccurred, this, &AnalyzerController::onScanError);

    connect(m_scanner, &DiskScanner::statusMessage, this, &AnalyzerController::onStatusMessage);

    connect(m_scanner, &DiskScanner::isScanningChanged, this, &AnalyzerController::isScanningChanged);

    connect(&AppSettings::instance(), &AppSettings::themeChanged, this, [this]() {

        m_themeManager.setTheme(AppSettings::instance().theme());

    });

}



bool AnalyzerController::isScanning() const

{

    return m_scanner->isScanning();

}



bool AnalyzerController::isAdmin() const

{

    return NtfsMftScanner::isElevated();

}



bool AnalyzerController::canUseMft() const

{

    QString reason;

    return NtfsMftScanner::canUseMftScan(m_scanPath, reason);

}



void AnalyzerController::setScanPath(const QString& path)

{

    const QString normalized = QDir::fromNativeSeparators(path);

    if (m_scanPath != normalized) {

        m_scanPath = normalized;

        emit scanPathChanged();

    }

}



void AnalyzerController::setMinFileSizeFilter(qint64 bytes)

{

    if (m_minFileSizeFilter != bytes) {

        m_minFileSizeFilter = bytes;

        AppSettings::instance().setMinFileSize(bytes);

        emit minFileSizeFilterChanged();

        rebuildFilteredTopFiles();

    }

}



void AnalyzerController::setSearchFilter(const QString& filter)

{

    if (m_searchFilter != filter) {

        m_searchFilter = filter;

        AppSettings::instance().setSearchFilter(filter);

        emit searchFilterChanged();

        rebuildFilteredTopFiles();

        syncFileIndexFilters();

    }

}



void AnalyzerController::setRegexSearch(bool enabled)

{

    if (m_regexSearch != enabled) {

        m_regexSearch = enabled;

        emit regexSearchChanged();

        syncFileIndexFilters();

    }

}



void AnalyzerController::setUseAllocatedSize(bool value)

{

    if (m_useAllocatedSize != value) {

        m_useAllocatedSize = value;

        m_fileIndexModel.setUseAllocatedSize(value);

        emit useAllocatedSizeChanged();

        updateTreemap();

    }

}



void AnalyzerController::setExtensionFilter(const QString& ext)

{

    if (m_extensionFilter != ext) {

        m_extensionFilter = ext;

        m_fileIndexModel.setExtensionFilter(ext);

        emit extensionFilterChanged();

        syncFileIndexFilters();

    }

}



void AnalyzerController::setCurrentView(int view)

{

    if (m_currentView != view) {

        m_currentView = view;

        emit currentViewChanged();

    }

}



void AnalyzerController::setTreemapFocusPath(const QString& path)

{

    if (m_treemapFocusPath != path) {

        m_treemapFocusPath = path;

        emit treemapFocusPathChanged();

        updateTreemap();

    }

}



void AnalyzerController::syncFileIndexFilters()

{

    m_fileIndexModel.setFilterText(m_searchFilter);

    m_fileIndexModel.setRegexFilter(m_searchFilter, m_regexSearch);

}



void AnalyzerController::updateAvailableDrives()

{

    m_availableDrives.clear();

    const QFileInfoList drives = QDir::drives();

    for (const QFileInfo& drive : drives) {

        if (drive.exists() && drive.isReadable()) {

            m_availableDrives << drive.absolutePath();

        }

    }

    emit availableDrivesChanged();

}



void AnalyzerController::startScan()

{

    if (m_scanPath.isEmpty()) {

        setLastError(QStringLiteral("请先选择要扫描的路径"));

        emit scanError(m_lastError);

        return;

    }



    QFileInfo info(m_scanPath);

    if (!info.exists()) {

        setLastError(QStringLiteral("路径不存在: %1").arg(m_scanPath));

        emit scanError(m_lastError);

        return;

    }



    m_progressThrottleTimer.stop();

    m_hasPendingProgress = false;

    m_hasPendingScanResult = false;

    clearResults();

    setLastError(QString());

    setStatusMessage(QStringLiteral("正在扫描…"));

    m_scanTimer.start();

    m_rateTimer.start();

    m_lastRateBytes = 0;

    m_scanProgressIndeterminate = true;

    emit scanProgressChanged();



    ScanOptions options = ScanOptions::defaultForPath(m_scanPath);
    AppSettings::instance().applyToScanOptions(options);
    options.minFileSize = m_minFileSizeFilter;

    AppSettings::instance().setLastScanPath(m_scanPath);

    saveSettings();



    m_scanner->startScan(m_scanPath, m_minFileSizeFilter, AppSettings::instance().preferMftScan());

}



void AnalyzerController::stopScan()

{

    m_scanner->stopScan();

    setStatusMessage(QStringLiteral("扫描已停止"));

}



QString AnalyzerController::pickFolder()

{

    const QString dir = QFileDialog::getExistingDirectory(

        nullptr,

        QStringLiteral("选择扫描目录"),

        m_scanPath.isEmpty() ? QDir::homePath() : m_scanPath);

    if (!dir.isEmpty()) {

        setScanPath(dir);

    }

    return dir;

}



QString AnalyzerController::pickOpenFile()

{

    return QFileDialog::getOpenFileName(nullptr, QStringLiteral("打开文件"),

                                        m_scanPath, QStringLiteral("扫描快照 (*.cqtscan);;所有文件 (*.*)"));

}



QString AnalyzerController::pickSaveFile(const QString& suggestedName)

{

    return QFileDialog::getSaveFileName(nullptr, QStringLiteral("导出报告"), suggestedName);

}



void AnalyzerController::deleteSelectedFiles(const QStringList& filePaths)

{

    QStringList validPaths;
    validPaths.reserve(filePaths.size());
    for (const QString& path : filePaths) {
        const QString trimmed = path.trimmed();
        if (!trimmed.isEmpty()) {
            validPaths.append(trimmed);
        }
    }
    if (validPaths.isEmpty()) {
        return;
    }

    QString error;

    if (!m_scanner->deleteFilesToRecycleBin(validPaths, &error)) {

        setLastError(error.isEmpty() ? QStringLiteral("删除失败") : error);

        emit scanError(m_lastError);

        return;

    }

    setStatusMessage(QStringLiteral("已移至回收站"));

    if (!m_selectedFolderPath.isEmpty()) {

        loadFilesForFolder(m_selectedFolderPath);

    }

}



void AnalyzerController::openPath(const QString& path)

{

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));

}



void AnalyzerController::revealInExplorer(const QString& path)

{

    QFileInfo info(path);

    const QString target = info.isDir() ? path : info.absolutePath();

    QDesktopServices::openUrl(QUrl::fromLocalFile(target));

}



void AnalyzerController::copyPathToClipboard(const QString& path)

{

    if (QGuiApplication::clipboard()) {

        QGuiApplication::clipboard()->setText(path);

        setStatusMessage(QStringLiteral("路径已复制"));

    }

}



void AnalyzerController::selectFolderInTree(const QString& folderPath)

{

    m_selectedFolderPath = QDir::fromNativeSeparators(folderPath);

    emit selectedFolderPathChanged();

    loadFilesForFolder(m_selectedFolderPath);

    updateTreemap();

}



void AnalyzerController::drillTreemap(const QString& folderPath)

{

    selectFolderInTree(folderPath);

    setTreemapFocusPath(folderPath);

}



void AnalyzerController::treemapGoUp()

{

    if (m_treemapFocusPath.isEmpty() || !m_treeRoot) {

        return;

    }

    if (m_treemapFocusPath == m_treeRoot->path) {

        return;

    }

    QFileInfo info(m_treemapFocusPath);

    const QString parent = QDir::fromNativeSeparators(info.path());

    setTreemapFocusPath(parent);

    selectFolderInTree(parent);

}



bool AnalyzerController::exportCsv(const QString& filePath)

{

    QString error;

    if (!ReportExporter::exportCsv(filePath, m_treeRoot, m_topFilesRaw, &error)) {

        setLastError(error);

        emit scanError(error);

        return false;

    }

    setStatusMessage(QStringLiteral("已导出 CSV"));

    return true;

}



bool AnalyzerController::exportHtml(const QString& filePath)

{

    QString error;

    if (!ReportExporter::exportHtml(filePath, m_treeRoot, m_topFilesRaw, m_totalSize, &error)) {

        setLastError(error);

        emit scanError(error);

        return false;

    }

    setStatusMessage(QStringLiteral("已导出 HTML"));

    return true;

}



bool AnalyzerController::saveSnapshot(const QString& filePath)

{

    QString path = filePath;

    if (path.isEmpty()) {

        path = pickSaveFile(QStringLiteral("scan.cqtscan"));

    }

    if (path.isEmpty()) {

        return false;

    }

    QString error;

    m_lastScanResult.root = m_treeRoot;

    m_lastScanResult.topFiles = m_topFilesRaw;

    m_lastScanResult.fileIndex = m_fileIndex;

    m_lastScanResult.extensionStats = m_extensionStats;

    m_lastScanResult.duplicateGroups = m_duplicateGroups;

    m_lastScanResult.totalSize = m_totalSize;

    m_lastScanResult.totalAllocatedSize = m_totalAllocatedSize;

    m_lastScanResult.totalFiles = m_totalFiles;

    m_lastScanResult.directoriesScanned = m_directoriesScanned;

    m_lastScanResult.scanPath = m_scanPath;

    m_lastScanResult.usedMft = m_usedMftScan;

    if (!ScanSnapshot::save(path, m_lastScanResult, &error)) {

        setLastError(error);

        emit scanError(error);

        return false;

    }

    setStatusMessage(QStringLiteral("扫描快照已保存"));

    return true;

}



bool AnalyzerController::loadSnapshot(const QString& filePath)

{

    QString path = filePath;

    if (path.isEmpty()) {

        path = pickOpenFile();

    }

    if (path.isEmpty()) {

        return false;

    }

    ScanResult result;

    QString error;

    if (!ScanSnapshot::load(path, &result, &error)) {

        setLastError(error);

        emit scanError(error);

        return false;

    }

    if (!result.scanPath.isEmpty()) {

        setScanPath(result.scanPath);

    }

    applyScanResult(result, true);

    m_duplicateGroups = result.duplicateGroups;

    emit duplicateGroupsChanged();

    setStatusMessage(QStringLiteral("扫描快照已加载"));

    emit scanFinished();

    QTimer::singleShot(0, this, &AnalyzerController::finalizeScanPresentation);

    return true;

}



void AnalyzerController::saveSettings()

{

    AppSettings::instance().setLastScanPath(m_scanPath);

    AppSettings::instance().setMinFileSize(m_minFileSizeFilter);

    AppSettings::instance().setSearchFilter(m_searchFilter);

    AppSettings::instance().setTheme(m_themeManager.theme());

    AppSettings::instance().save();

}



void AnalyzerController::scanEmptyDirectories()

{

    runCleanupModule(QStringLiteral("emptyDirs"));

}



void AnalyzerController::scanDuplicateCandidates()

{

    runCleanupModule(QStringLiteral("duplicates"));

}



void AnalyzerController::runCleanupModule(const QString& moduleId)

{

    CleanupScanOptions opts;

    opts.rootPath = m_scanPath;

    opts.minFileSize = m_minFileSizeFilter > 0 ? m_minFileSizeFilter : (1024 * 1024);

    m_cleanupResults = ScanModuleRegistry::instance().runModule(moduleId, opts);

    if (moduleId == QStringLiteral("duplicates")) {

        m_duplicateGroups = m_cleanupResults;

        emit duplicateGroupsChanged();

    }

    emit cleanupResultsChanged();

    setStatusMessage(ScanModuleRegistry::instance().displayNameFor(moduleId)

                     + QStringLiteral(" 扫描完成"));

}



void AnalyzerController::restartAsAdmin()

{

    if (UacHelper::restartElevated()) {

        QCoreApplication::quit();

    } else {

        setLastError(QStringLiteral("无法以管理员身份重启"));

        emit scanError(m_lastError);

    }

}



void AnalyzerController::runCustomCleanupOnSelection(const QStringList& paths)

{

    const QString command = AppSettings::instance().customCleanupCommand();

    if (command.isEmpty() || paths.isEmpty()) {

        return;

    }

    for (const QString& path : paths) {

        QString cmdLine = command;

        cmdLine.replace(QStringLiteral("%PATH%"), path);

        QProcess::startDetached(QStringLiteral("cmd.exe"), {QStringLiteral("/c"), cmdLine});

    }

    setStatusMessage(QStringLiteral("已运行自定义清理命令"));

}



QString AnalyzerController::formatFileSize(qint64 bytes) const

{

    return FormatUtils::formatFileSize(bytes);

}



void AnalyzerController::clearResults()

{

    m_folderTreeModel.clear();

    m_fileIndexModel.clear();

    m_treeRoot.reset();

    m_fileIndex.clear();

    m_filesByParent.clear();

    m_topFiles.clear();

    m_filteredTopFiles.clear();

    m_topFilesRaw.clear();

    m_extensionStats.clear();

    m_topLevelFolders.clear();

    m_selectedFolderFiles.clear();

    m_duplicateGroups.clear();

    m_selectedFolderPath.clear();

    m_treemapFocusPath.clear();

    m_totalFiles = 0;

    m_totalSize = 0;

    m_totalAllocatedSize = 0;

    m_scanRateBytesPerSec = 0;

    m_directoriesScanned = 0;

    m_skippedPaths = 0;

    m_scanProgress = 0;

    m_scanElapsedMs = 0;

    m_usedMftScan = false;



    emit topFilesChanged();

    emit filteredTopFilesChanged();

    emit extensionStatsChanged();

    emit topLevelFoldersChanged();

    emit selectedFolderFilesChanged();

    emit duplicateGroupsChanged();

    emit selectedFolderPathChanged();

    emit treemapFocusPathChanged();

    emit totalFilesChanged();

    emit totalSizeChanged();

    emit totalAllocatedSizeChanged();

    emit scanRateChanged();

    emit directoriesScannedChanged();

    emit skippedPathsChanged();

    emit scanProgressChanged();

    emit scanElapsedMsChanged();

    emit usedMftScanChanged();

    updateTreemap();

}



void AnalyzerController::applyScanResult(const ScanResult& result, bool finalPass)

{

    Q_UNUSED(finalPass);

    m_treeRoot = result.root;

    m_topFilesRaw = result.topFiles;

    m_fileIndex = result.fileIndex;

    m_totalFiles = result.totalFiles;

    m_totalSize = result.totalSize;

    m_totalAllocatedSize = result.totalAllocatedSize > 0 ? result.totalAllocatedSize

        : result.totalSize;

    m_directoriesScanned = result.directoriesScanned;

    m_skippedPaths = result.skippedPaths;

    m_extensionStats = result.extensionStats;

    m_lastScanResult = result;

    if (result.usedMft) {

        m_usedMftScan = true;

    }



    m_folderTreeModel.setTree(m_treeRoot, m_totalSize);

    m_fileIndexModel.setEntries(m_fileIndex);

    syncFileIndexFilters();

    m_topFiles = fileInfoVectorToVariantList(m_topFilesRaw);

    rebuildFilteredTopFiles();

    rebuildTopLevelFolders();

    rebuildFilesByParentCache();

    emit topFilesChanged();

    emit filteredTopFilesChanged();

    emit extensionStatsChanged();

    emit topLevelFoldersChanged();

    emit totalFilesChanged();

    emit totalSizeChanged();

    emit totalAllocatedSizeChanged();

    emit directoriesScannedChanged();

    emit skippedPathsChanged();

    emit usedMftScanChanged();

}



void AnalyzerController::rebuildFilesByParentCache()

{

    m_filesByParent.clear();

    m_filesByParent.reserve(m_fileIndex.size() / 4 + 1);

    for (const FileIndexEntry& entry : m_fileIndex) {

        const QString parent = QDir::fromNativeSeparators(QFileInfo(entry.path).path());

        m_filesByParent[parent].append(entry);

    }

    for (auto it = m_filesByParent.begin(); it != m_filesByParent.end(); ++it) {

        auto& files = it.value();

        std::sort(files.begin(), files.end(),

                  [](const FileIndexEntry& a, const FileIndexEntry& b) {

                      return a.logicalSize > b.logicalSize;

                  });

    }

}



QVector<FileIndexEntry> AnalyzerController::directFilesForFocus() const

{

    if (m_treemapFocusPath.isEmpty()) {

        return {};

    }

    const QString focus = QDir::fromNativeSeparators(m_treemapFocusPath);

    const auto it = m_filesByParent.constFind(focus);

    if (it == m_filesByParent.constEnd()) {

        return {};

    }

    QVector<FileIndexEntry> files = it.value();

    if (files.size() > 200) {

        files.resize(200);

    }

    return files;

}



void AnalyzerController::rebuildFilteredTopFiles()

{

    m_filteredTopFiles = filterFileVariantList(m_topFiles);

    emit filteredTopFilesChanged();

}



void AnalyzerController::rebuildTopLevelFolders()

{

    m_topLevelFolders.clear();

    if (!m_treeRoot) {

        emit topLevelFoldersChanged();

        return;

    }

    for (const auto& child : m_treeRoot->children) {

        QVariantMap map;

        map[QStringLiteral("name")] = child->name;

        map[QStringLiteral("path")] = child->path;

        map[QStringLiteral("size")] = child->size;

        const double pct = m_totalSize > 0 ? static_cast<double>(child->size) / static_cast<double>(m_totalSize) : 0.0;

        map[QStringLiteral("sizePercent")] = pct;

        m_topLevelFolders.append(map);

    }

    emit topLevelFoldersChanged();

}



void AnalyzerController::updateTreemap()

{

    if (m_treeRoot) {

        const QString focus = m_treemapFocusPath.isEmpty() ? m_treeRoot->path : m_treemapFocusPath;

        m_treemapLayout.buildFromNode(m_treeRoot, m_totalSize, focus, directFilesForFocus());

    }

}



QVariantList AnalyzerController::filterFileVariantList(const QVariantList& list) const

{

    if (m_searchFilter.trimmed().isEmpty()) {

        return list;

    }

    const QString needle = m_searchFilter.trimmed();

    QVariantList filtered;

    for (const QVariant& item : list) {

        const QVariantMap map = item.toMap();

        const QString path = map.value(QStringLiteral("path")).toString();

        if (m_regexSearch) {

            QRegularExpression re(needle);

            if (re.isValid() && re.match(path).hasMatch()) {

                filtered.append(item);

            }

        } else if (path.contains(needle, Qt::CaseInsensitive)) {

            filtered.append(item);

        }

    }

    return filtered;

}



void AnalyzerController::onScanProgress(int directoriesScanned, qint64 totalSize, int fileCount,
                                      int progressPercent)

{

    m_pendingDirectoriesScanned = directoriesScanned;

    m_pendingTotalSize = totalSize;

    m_pendingTotalFiles = fileCount;

    m_pendingProgressPercent = progressPercent;

    m_hasPendingProgress = true;

    if (!m_progressThrottleTimer.isActive()) {

        m_progressThrottleTimer.start();

    }

}



void AnalyzerController::flushThrottledScanProgress()

{

    if (!m_hasPendingProgress) {

        return;

    }

    m_hasPendingProgress = false;

    m_directoriesScanned = m_pendingDirectoriesScanned;

    m_totalSize = m_pendingTotalSize;

    m_totalFiles = m_pendingTotalFiles;

    if (m_rateTimer.isValid()) {

        const qint64 elapsed = m_rateTimer.elapsed();

        if (elapsed > 500) {

            const qint64 delta = m_pendingTotalSize - m_lastRateBytes;

            m_scanRateBytesPerSec = elapsed > 0 ? (delta * 1000.0) / elapsed : 0;

            m_lastRateBytes = m_pendingTotalSize;

            m_rateTimer.restart();

            emit scanRateChanged();

        }

    }

    if (m_pendingProgressPercent < 0) {

        m_scanProgressIndeterminate = true;

    } else {

        m_scanProgressIndeterminate = false;

        m_scanProgress = m_pendingProgressPercent;

    }

    emit directoriesScannedChanged();

    emit totalSizeChanged();

    emit totalFilesChanged();

    emit scanProgressChanged();

}



void AnalyzerController::onScanFinished(const ScanResult& result)

{

    if (m_hasPendingProgress) {

        flushThrottledScanProgress();

    }

    m_pendingScanResult = result;

    m_hasPendingScanResult = true;

    m_scanElapsedMs = m_scanTimer.elapsed();

    m_scanProgress = 100;

    m_scanProgressIndeterminate = false;

    m_scanRateBytesPerSec = 0;

    setStatusMessage(QStringLiteral("正在整理结果…"));

    emit scanProgressChanged();

    emit scanElapsedMsChanged();

    emit scanRateChanged();

    QTimer::singleShot(0, this, &AnalyzerController::applyPendingScanResult);

}



void AnalyzerController::applyPendingScanResult()

{

    if (!m_hasPendingScanResult) {

        return;

    }

    m_hasPendingScanResult = false;

    const ScanResult result = std::move(m_pendingScanResult);

    applyScanResult(result, true);

    setStatusMessage(result.usedMft ? QStringLiteral("MFT 扫描完成") : QStringLiteral("扫描完成"));

    emit scanFinished();

    emit isScanningChanged();

    QTimer::singleShot(0, this, &AnalyzerController::finalizeScanPresentation);

}



void AnalyzerController::finalizeScanPresentation()

{

    if (m_treeRoot) {

        m_treemapFocusPath = m_treeRoot->path;

        emit treemapFocusPathChanged();

        selectFolderInTree(m_treeRoot->path);

    }

    updateTreemap();

}



void AnalyzerController::onScanError(const QString& error)

{

    if (!error.isEmpty()) {

        setLastError(error);

        emit scanError(error);

    }

}



void AnalyzerController::onStatusMessage(const QString& message)

{

    setStatusMessage(message);

}



void AnalyzerController::setLastError(const QString& error)

{

    if (m_lastError != error) {

        m_lastError = error;

        emit lastErrorChanged();

    }

}



void AnalyzerController::setStatusMessage(const QString& message)

{

    if (m_statusMessage != message) {

        m_statusMessage = message;

        emit statusMessageChanged();

    }

}



void AnalyzerController::loadFilesForFolder(const QString& folderPath)

{

    m_selectedFolderFiles.clear();

    if (!m_fileIndex.isEmpty()) {

        const QString norm = QDir::fromNativeSeparators(folderPath);

        QVector<FileInfo> list;

        for (const FileIndexEntry& entry : m_fileIndex) {

            if (QDir::fromNativeSeparators(QFileInfo(entry.path).path()) == norm) {

                list.append(FileInfo(entry.path, entry.logicalSize, entry.lastModified, false));

            }

        }

        std::sort(list.begin(), list.end(),

                  [](const FileInfo& a, const FileInfo& b) { return a.size > b.size; });

        if (list.size() > 500) {

            list.resize(500);

        }

        m_selectedFolderFiles = filterFileVariantList(fileInfoVectorToVariantList(list));

        emit selectedFolderFilesChanged();

        return;

    }



    QDir dir(folderPath);

    if (!dir.exists()) {

        emit selectedFolderFilesChanged();

        return;

    }



    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

    const QFileInfoList files = dir.entryInfoList();

    QVector<FileInfo> list;

    list.reserve(files.size());

    for (const QFileInfo& entry : files) {

        if (entry.size() < m_minFileSizeFilter) {

            continue;

        }

        list.append(FileInfo(entry.absoluteFilePath(), entry.size(), entry.lastModified(), false));

    }

    std::sort(list.begin(), list.end(),

              [](const FileInfo& a, const FileInfo& b) { return a.size > b.size; });



    const int maxShow = 500;

    if (list.size() > maxShow) {

        list.resize(maxShow);

    }

    m_selectedFolderFiles = filterFileVariantList(fileInfoVectorToVariantList(list));

    emit selectedFolderFilesChanged();

}



QVariantList AnalyzerController::fileInfoVectorToVariantList(const QVector<FileInfo>& files) const

{

    QVariantList list;

    list.reserve(files.size());

    for (const FileInfo& info : files) {

        QVariantMap map;

        map[QStringLiteral("path")] = info.path;

        map[QStringLiteral("size")] = info.size;

        map[QStringLiteral("lastModified")] = info.lastModified.toString(Qt::ISODate);

        map[QStringLiteral("isDirectory")] = info.isDirectory;

        list.append(map);

    }

    return list;

}

