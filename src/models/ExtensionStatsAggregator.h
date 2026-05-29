#ifndef EXTENSIONSTATSAGGREGATOR_H
#define EXTENSIONSTATSAGGREGATOR_H

#include <QHash>
#include <QString>
#include <QVariantList>
#include <QMutex>
#include "ExtensionColorMap.h"

struct ExtensionStats {
    QString extension;
    qint64 totalSize = 0;
    int fileCount = 0;
    QString color;
};

class ExtensionStatsAggregator {
public:
    void addFile(const QString& filePath, qint64 size);
    QVariantList toVariantList() const;
    void clear();

private:
    mutable QMutex m_mutex;
    QHash<QString, ExtensionStats> m_stats;
};

#endif // EXTENSIONSTATSAGGREGATOR_H
