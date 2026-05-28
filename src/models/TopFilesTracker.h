#ifndef TOPFILESTRACKER_H
#define TOPFILESTRACKER_H

#include <QVector>
#include <algorithm>
#include "../FileInfo.h"

class TopFilesTracker {
public:
    explicit TopFilesTracker(int maxCount = 100);

    void consider(const FileInfo& info);
    QVector<FileInfo> sortedTopFiles() const;

private:
    int m_maxCount;
    QVector<FileInfo> m_items;
};

#endif // TOPFILESTRACKER_H
