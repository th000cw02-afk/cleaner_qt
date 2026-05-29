#include "TopFilesTracker.h"

TopFilesTracker::TopFilesTracker(int maxCount)
    : m_maxCount(maxCount)
{
}

void TopFilesTracker::consider(const FileInfo& info)
{
    if (m_items.size() < m_maxCount) {
        m_items.append(info);
        return;
    }
    auto minIt = std::min_element(m_items.begin(), m_items.end(),
                                  [](const FileInfo& a, const FileInfo& b) { return a.size < b.size; });
    if (minIt != m_items.end() && info.size > minIt->size) {
        *minIt = info;
    }
}

QVector<FileInfo> TopFilesTracker::sortedTopFiles() const
{
    QVector<FileInfo> result = m_items;
    std::sort(result.begin(), result.end(),
              [](const FileInfo& a, const FileInfo& b) { return a.size > b.size; });
    return result;
}
