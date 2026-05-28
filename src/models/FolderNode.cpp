#include "FolderNode.h"
#include <QDir>
#include <QFileInfo>
#include <algorithm>

FolderNode::FolderNode(const QString& nodePath, FolderNode* parentNode)
    : path(QDir::fromNativeSeparators(nodePath))
    , parent(parentNode)
{
    QFileInfo info(path);
    name = info.fileName();
    if (name.isEmpty()) {
        name = path;
    }
}

void FolderNode::addChild(const std::shared_ptr<FolderNode>& child)
{
    child->parent = this;
    children.append(child);
    dirCount = children.size();
}

void FolderNode::sortChildrenBySize()
{
    std::sort(children.begin(), children.end(),
              [](const std::shared_ptr<FolderNode>& a, const std::shared_ptr<FolderNode>& b) {
                  return a->size > b->size;
              });
    for (const auto& child : children) {
        child->sortChildrenBySize();
    }
}

FolderTreeBuilder::FolderTreeBuilder(const QString& rootPath)
{
    m_rootPath = QDir::fromNativeSeparators(QFileInfo(rootPath).absoluteFilePath());
    m_root = std::make_shared<FolderNode>(m_rootPath);
}

std::shared_ptr<FolderNode> FolderTreeBuilder::root() const
{
    QMutexLocker lock(&m_mutex);
    return m_root;
}

std::shared_ptr<FolderNode> FolderTreeBuilder::ensureFolder(const QString& folderPath)
{
    QMutexLocker lock(&m_mutex);
    return ensureFolderUnlocked(folderPath);
}

std::shared_ptr<FolderNode> FolderTreeBuilder::ensureFolderUnlocked(const QString& folderPath)
{
    QString norm = QDir::fromNativeSeparators(QFileInfo(folderPath).absoluteFilePath());
    if (norm == m_rootPath) {
        return m_root;
    }

    std::shared_ptr<FolderNode> current = m_root;
    QString relative = norm.mid(m_rootPath.length());
    if (relative.startsWith(QLatin1Char('/'))) {
        relative = relative.mid(1);
    }
    if (relative.isEmpty()) {
        return m_root;
    }

    const QStringList parts = relative.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    QString built = m_rootPath;
    for (const QString& part : parts) {
        built += QLatin1Char('/') + part;
        std::shared_ptr<FolderNode> next;
        for (const auto& child : current->children) {
            if (child->path == built) {
                next = child;
                break;
            }
        }
        if (!next) {
            next = std::make_shared<FolderNode>(built, current.get());
            current->addChild(next);
        }
        current = next;
    }
    return current;
}

void FolderTreeBuilder::addFileSize(const QString& filePath, qint64 size)
{
    addFileSize(filePath, size, size, QDateTime());
}

void FolderTreeBuilder::addFileSize(const QString& filePath, qint64 logicalSize,
                                    qint64 allocatedSize, const QDateTime& modified)
{
    QMutexLocker lock(&m_mutex);
    QFileInfo fi(filePath);
    QString dirPath = QDir::fromNativeSeparators(fi.absolutePath());
    ensureFolderUnlocked(dirPath);

    QString current = dirPath;
    while (!current.isEmpty() && current.length() >= m_rootPath.length()) {
        if (!current.startsWith(m_rootPath)) {
            break;
        }
        auto node = ensureFolderUnlocked(current);
        node->size += logicalSize;
        node->allocatedSize += allocatedSize;
        node->fileCount++;
        if (modified.isValid() && (!node->lastModified.isValid() || modified > node->lastModified)) {
            node->lastModified = modified;
        }
        if (current == m_rootPath) {
            break;
        }
        current = QDir::fromNativeSeparators(QFileInfo(current).path());
    }
}
