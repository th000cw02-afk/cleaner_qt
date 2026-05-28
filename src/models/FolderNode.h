#ifndef FOLDERNODE_H
#define FOLDERNODE_H

#include <QString>
#include <QVector>
#include <QRecursiveMutex>
#include <QDateTime>
#include <memory>

class FolderNode {
public:
    QString path;
    QString name;
    qint64 size = 0;
    qint64 allocatedSize = 0;
    QDateTime lastModified;
    int fileCount = 0;
    int dirCount = 0;
    FolderNode* parent = nullptr;
    QVector<std::shared_ptr<FolderNode>> children;

    explicit FolderNode(const QString& nodePath, FolderNode* parentNode = nullptr);

    void addChild(const std::shared_ptr<FolderNode>& child);
    void sortChildrenBySize();
};

class FolderTreeBuilder {
public:
    explicit FolderTreeBuilder(const QString& rootPath);

    std::shared_ptr<FolderNode> root() const;
    void addFileSize(const QString& filePath, qint64 size);
    void addFileSize(const QString& filePath, qint64 logicalSize, qint64 allocatedSize,
                     const QDateTime& modified);
    std::shared_ptr<FolderNode> ensureFolder(const QString& folderPath);

private:
    QString m_rootPath;
    std::shared_ptr<FolderNode> m_root;
    mutable QRecursiveMutex m_mutex;
    std::shared_ptr<FolderNode> ensureFolderUnlocked(const QString& folderPath);
};

#endif // FOLDERNODE_H
