#ifndef PARALLELSCANWORKER_H

#define PARALLELSCANWORKER_H



#include <QObject>

#include <QMutex>

#include "ScanOptions.h"

#include "../models/FolderNode.h"

#include "../models/TopFilesTracker.h"

#include "../models/FileIndexBuilder.h"

#include "ScanWorker.h"



class ParallelScanWorker : public QObject {

    Q_OBJECT



public:

    explicit ParallelScanWorker(QObject* parent = nullptr);



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

    int m_skippedPaths = 0;

    QMutex m_mutex;



    void scanDirectoryRecursive(const QString& path, const ScanOptions& options,

                                FolderTreeBuilder& builder, TopFilesTracker& topFiles,

                                FileIndexBuilder& fileIndexBuilder,

                                class ExtensionStatsAggregator& extStats,

                                int& directoriesScanned, qint64& totalSize, qint64& totalAllocated,

                                int& totalFiles, qint64 volumeBytes);

    static constexpr int kProgressEmitFileInterval = 1000;

};



#endif // PARALLELSCANWORKER_H

