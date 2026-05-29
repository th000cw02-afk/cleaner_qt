#include "ExtensionStatsAggregator.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <algorithm>

void ExtensionStatsAggregator::addFile(const QString& filePath, qint64 size)
{
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();
    if (ext.isEmpty()) {
        ext = QStringLiteral("(无扩展名)");
    } else {
        ext = QLatin1Char('.') + ext;
    }

    QMutexLocker lock(&m_mutex);
    ExtensionStats& stats = m_stats[ext];
    stats.extension = ext;
    stats.color = ExtensionColorMap::colorForExtension(ext);
    stats.totalSize += size;
    stats.fileCount++;
}

QVariantList ExtensionStatsAggregator::toVariantList() const
{
    QMutexLocker lock(&m_mutex);
    QVector<ExtensionStats> sorted;
    sorted.reserve(m_stats.size());
    for (auto it = m_stats.constBegin(); it != m_stats.constEnd(); ++it) {
        sorted.append(it.value());
    }
    std::sort(sorted.begin(), sorted.end(),
              [](const ExtensionStats& a, const ExtensionStats& b) {
                  return a.totalSize > b.totalSize;
              });

    QVariantList list;
    for (const ExtensionStats& stats : sorted) {
        QVariantMap map;
        map[QStringLiteral("extension")] = stats.extension;
        map[QStringLiteral("totalSize")] = stats.totalSize;
        map[QStringLiteral("fileCount")] = stats.fileCount;
        map[QStringLiteral("color")] = stats.color;
        list.append(map);
    }
    return list;
}

void ExtensionStatsAggregator::clear()
{
    QMutexLocker lock(&m_mutex);
    m_stats.clear();
}
