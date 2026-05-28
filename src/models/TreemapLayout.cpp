#include "TreemapLayout.h"
#include <QDir>
#include <QFileInfo>
#include <QVariantMap>
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

TreemapLayout::TreemapLayout(QObject* parent)
    : QObject(parent)
{
}

QVariantList TreemapLayout::rects() const
{
    return m_rectsVariant;
}

void TreemapLayout::setFocusPath(const QString& path)
{
    if (m_focusPath != path) {
        m_focusPath = path;
        emit focusPathChanged();
        if (m_root) {
            buildFromNode(m_root, m_totalSize, m_focusPath);
        }
    }
}

void TreemapLayout::buildFromNode(const std::shared_ptr<FolderNode>& root, qint64 totalSize,
                                  const QString& focusPath,
                                  const QVector<FileIndexEntry>& directFiles)
{
    m_root = root;
    m_totalSize = totalSize > 0 ? totalSize : (root ? root->size : 0);
    m_focusPath = focusPath;
    m_rects.clear();

    if (!root) {
        m_rectsVariant.clear();
        emit rectsChanged();
        return;
    }

    std::shared_ptr<FolderNode> focusNode = root;
    if (!focusPath.isEmpty() && focusPath != root->path) {
        std::function<std::shared_ptr<FolderNode>(const std::shared_ptr<FolderNode>&)> find;
        find = [&](const std::shared_ptr<FolderNode>& node) -> std::shared_ptr<FolderNode> {
            if (node->path == focusPath) {
                return node;
            }
            for (const auto& child : node->children) {
                if (auto found = find(child)) {
                    return found;
                }
            }
            return nullptr;
        };
        if (auto found = find(root)) {
            focusNode = found;
        }
    }

    QVector<std::shared_ptr<FolderNode>> childNodes;
    childNodes.reserve(focusNode->children.size());
    for (const auto& child : focusNode->children) {
        if (child->size > 0) {
            childNodes.append(child);
        }
    }

    const QString focusNorm = QDir::fromNativeSeparators(focusNode->path);
    QVector<TreemapRect> fileRects;
    for (const FileIndexEntry& file : directFiles) {
        const QString parent = QDir::fromNativeSeparators(QFileInfo(file.path).path());
        if (parent != focusNorm || file.logicalSize <= 0) {
            continue;
        }
        TreemapRect rect;
        rect.path = file.path;
        rect.name = QFileInfo(file.path).fileName();
        rect.size = file.logicalSize;
        rect.color = colorForExtension(file.extension);
        fileRects.append(rect);
    }

    const qint64 focusTotal = focusNode->size > 0 ? focusNode->size : 1;
    squarify(childNodes, focusTotal, QRectF(0, 0, 1, 1), m_rects);

    if (!fileRects.isEmpty()) {
        qint64 fileTotal = 0;
        for (const TreemapRect& r : fileRects) {
            fileTotal += r.size;
        }
        const qreal fileArea = qMin<qreal>(0.35, static_cast<qreal>(fileTotal) / focusTotal);
        if (fileArea > 0.02) {
            const QRectF fileRegion(1.0 - fileArea, 0, fileArea, 1.0);
            QVector<std::shared_ptr<FolderNode>> pseudoNodes;
            pseudoNodes.reserve(fileRects.size());
            for (const TreemapRect& fr : fileRects) {
                auto pseudo = std::make_shared<FolderNode>(fr.path);
                pseudo->name = fr.name;
                pseudo->size = fr.size;
                pseudoNodes.append(pseudo);
            }
            QVector<TreemapRect> fileLayout;
            squarify(pseudoNodes, fileTotal, fileRegion, fileLayout);
            for (int i = 0; i < fileLayout.size() && i < fileRects.size(); ++i) {
                fileLayout[i].color = fileRects.at(i).color;
            }
            m_rects.append(fileLayout);

            for (TreemapRect& r : m_rects) {
                if (r.rect.x() + r.rect.width() > 1.0 - fileArea - 0.001) {
                    continue;
                }
                r.rect.setWidth(r.rect.width() * (1.0 - fileArea));
            }
        }
    }

    for (TreemapRect& r : m_rects) {
        if (r.color.isEmpty()) {
            r.color = colorForName(r.name);
        }
        r.sizePercent = static_cast<double>(r.size) / static_cast<double>(focusTotal);
    }

    m_rectsVariant.clear();
    for (const TreemapRect& r : m_rects) {
        QVariantMap map;
        map[QStringLiteral("path")] = r.path;
        map[QStringLiteral("name")] = r.name;
        map[QStringLiteral("size")] = r.size;
        map[QStringLiteral("sizePercent")] = r.sizePercent;
        map[QStringLiteral("x")] = r.rect.x();
        map[QStringLiteral("y")] = r.rect.y();
        map[QStringLiteral("w")] = r.rect.width();
        map[QStringLiteral("h")] = r.rect.height();
        map[QStringLiteral("color")] = r.color;
        m_rectsVariant.append(map);
    }
    emit rectsChanged();
}

void TreemapLayout::squarify(const QVector<std::shared_ptr<FolderNode>>& nodes,
                             qint64 totalSize,
                             const QRectF& area,
                             QVector<TreemapRect>& output)
{
    if (nodes.isEmpty() || totalSize <= 0) {
        return;
    }

    qreal x = area.x();
    qreal y = area.y();
    qreal w = area.width();
    qreal h = area.height();

    int index = 0;
    const bool horizontal = w >= h;

    while (index < nodes.size()) {
        qint64 rowSize = 0;
        const int rowStart = index;
        qreal bestAspect = std::numeric_limits<qreal>::max();
        int rowEnd = index;

        while (rowEnd < nodes.size()) {
            const qint64 candidateSize = rowSize + nodes[rowEnd]->size;
            const int count = rowEnd - rowStart + 1;
            const qreal rowThickness = horizontal
                ? (static_cast<qreal>(candidateSize) / totalSize) * w
                : (static_cast<qreal>(candidateSize) / totalSize) * h;
            const qreal maxSide = horizontal ? h / count : w / count;
            const qreal minSide = horizontal
                ? (static_cast<qreal>(nodes[rowStart]->size) / candidateSize) * h
                : (static_cast<qreal>(nodes[rowStart]->size) / candidateSize) * w;
            const qreal aspect = qMax(rowThickness / qMax(minSide, 0.0001),
                                     maxSide / qMax(rowThickness, 0.0001));
            if (aspect <= bestAspect) {
                bestAspect = aspect;
                rowSize = candidateSize;
                rowEnd++;
            } else {
                break;
            }
        }
        if (rowEnd == rowStart) {
            rowEnd = rowStart + 1;
            rowSize = nodes[rowStart]->size;
        }

        const qreal rowThickness = horizontal
            ? (static_cast<qreal>(rowSize) / totalSize) * w
            : (static_cast<qreal>(rowSize) / totalSize) * h;

        qreal offset = 0;
        for (int i = rowStart; i < rowEnd; ++i) {
            const auto& node = nodes[i];
            const qreal fraction = static_cast<qreal>(node->size) / rowSize;
            TreemapRect rect;
            rect.path = node->path;
            rect.name = node->name;
            rect.size = node->size;
            rect.color = colorForName(node->name);

            if (horizontal) {
                const qreal sliceH = fraction * h;
                rect.rect = QRectF(x, y + offset, rowThickness, sliceH);
                offset += sliceH;
            } else {
                const qreal sliceW = fraction * w;
                rect.rect = QRectF(x + offset, y, sliceW, rowThickness);
                offset += sliceW;
            }
            output.append(rect);
        }

        if (horizontal) {
            x += rowThickness;
            w -= rowThickness;
        } else {
            y += rowThickness;
            h -= rowThickness;
        }
        index = rowEnd;
    }
}

QString TreemapLayout::colorForName(const QString& name)
{
    const uint hash = qHash(name);
    const QStringList palette = {
        QStringLiteral("#42a5f5"), QStringLiteral("#66bb6a"), QStringLiteral("#ffa726"),
        QStringLiteral("#ab47bc"), QStringLiteral("#ef5350"), QStringLiteral("#26c6da"),
        QStringLiteral("#8d6e63"), QStringLiteral("#7e57c2"), QStringLiteral("#29b6f6")
    };
    return palette.at(hash % palette.size());
}

QString TreemapLayout::colorForExtension(const QString& extension)
{
    return ExtensionColorMap::colorForExtension(extension);
}

int TreemapLayout::hitTest(qreal x, qreal y) const
{
    for (int i = 0; i < m_rects.size(); ++i) {
        if (m_rects.at(i).rect.contains(x, y)) {
            return i;
        }
    }
    return -1;
}

QString TreemapLayout::pathAt(int index) const
{
    if (index < 0 || index >= m_rects.size()) {
        return QString();
    }
    return m_rects.at(index).path;
}
