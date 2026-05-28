#include "FolderTreeModel.h"

FolderTreeModel::FolderTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void FolderTreeModel::setTree(std::shared_ptr<FolderNode> root, qint64 totalSize)
{
    beginResetModel();
    m_root = std::move(root);
    m_rootSize = totalSize > 0 ? totalSize : (m_root ? m_root->size : 0);
    endResetModel();
    emit rootChanged();
}

void FolderTreeModel::clear()
{
    beginResetModel();
    m_root.reset();
    m_rootSize = 0;
    endResetModel();
    emit rootChanged();
}

FolderNode* FolderTreeModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return m_root.get();
    }
    return static_cast<FolderNode*>(index.internalPointer());
}

QModelIndex FolderTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_root || row < 0 || column < 0 || column >= columnCount()) {
        return QModelIndex();
    }

    FolderNode* parentNode = nodeFromIndex(parent);
    if (!parentNode || row >= parentNode->children.size()) {
        return QModelIndex();
    }

    return createIndex(row, column, parentNode->children.at(row).get());
}

QModelIndex FolderTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid() || !m_root) {
        return QModelIndex();
    }

    FolderNode* child = nodeFromIndex(index);
    if (!child || !child->parent || child->parent == m_root.get()) {
        return QModelIndex();
    }

    FolderNode* parentNode = child->parent;
    FolderNode* grandParent = parentNode->parent;
    if (!grandParent) {
        return QModelIndex();
    }

    for (int i = 0; i < grandParent->children.size(); ++i) {
        if (grandParent->children.at(i).get() == parentNode) {
            return createIndex(i, 0, parentNode);
        }
    }
    return QModelIndex();
}

int FolderTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!m_root) {
        return 0;
    }
    FolderNode* parentNode = nodeFromIndex(parent);
    if (!parentNode) {
        return 0;
    }
    return parentNode->children.size();
}

int FolderTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant FolderTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_root) {
        return QVariant();
    }

    FolderNode* node = nodeFromIndex(index);
    if (!node) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return node->name;
    case PathRole:
        return node->path;
    case SizeRole:
        return node->size;
    case AllocatedSizeRole:
        return node->allocatedSize;
    case LastModifiedRole:
        return node->lastModified;
    case FileCountRole:
        return node->fileCount;
    case DirCountRole:
        return node->dirCount;
    case SizePercentRole:
        if (m_rootSize <= 0) {
            return 0.0;
        }
        return static_cast<double>(node->size) / static_cast<double>(m_rootSize);
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> FolderTreeModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {SizeRole, "size"},
        {AllocatedSizeRole, "allocatedSize"},
        {LastModifiedRole, "lastModified"},
        {FileCountRole, "fileCount"},
        {DirCountRole, "dirCount"},
        {SizePercentRole, "sizePercent"}
    };
}
