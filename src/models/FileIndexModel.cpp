#include "FileIndexModel.h"
#include <algorithm>

FileIndexModel::FileIndexModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void FileIndexModel::setUseAllocatedSize(bool value)
{
    if (m_useAllocatedSize != value) {
        m_useAllocatedSize = value;
        if (rowCount() > 0) {
            emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        }
        emit useAllocatedSizeChanged();
    }
}

void FileIndexModel::setEntries(QVector<FileIndexEntry> entries)
{
    beginResetModel();
    m_entries = std::move(entries);
    rebuildFilter();
    endResetModel();
    emit filterChanged();
}

void FileIndexModel::clear()
{
    beginResetModel();
    m_entries.clear();
    m_filteredIndices.clear();
    endResetModel();
    emit filterChanged();
}

void FileIndexModel::setFilterText(const QString& text)
{
    if (m_filterText != text) {
        m_filterText = text;
        beginResetModel();
        rebuildFilter();
        endResetModel();
        emit filterChanged();
    }
}

void FileIndexModel::setRegexFilter(const QString& pattern, bool enabled)
{
    m_regexEnabled = enabled;
    m_regexFilter = QRegularExpression(pattern);
    beginResetModel();
    rebuildFilter();
    endResetModel();
    emit filterChanged();
}

void FileIndexModel::setExtensionFilter(const QString& extension)
{
    if (m_extensionFilter != extension) {
        m_extensionFilter = extension;
        beginResetModel();
        rebuildFilter();
        endResetModel();
        emit filterChanged();
    }
}

void FileIndexModel::rebuildFilter()
{
    m_filteredIndices.clear();
    m_filteredIndices.reserve(m_entries.size());
    for (int i = 0; i < m_entries.size(); ++i) {
        const FileIndexEntry& e = m_entries.at(i);
        if (!m_extensionFilter.isEmpty() && e.extension != m_extensionFilter) {
            continue;
        }
        if (m_regexEnabled && m_regexFilter.isValid()) {
            if (!m_regexFilter.match(e.path).hasMatch()) {
                continue;
            }
        } else if (!m_filterText.isEmpty()
                   && !e.path.contains(m_filterText, Qt::CaseInsensitive)) {
            continue;
        }
        m_filteredIndices.append(i);
    }
}

const FileIndexEntry& FileIndexModel::entryAtFilteredRow(int row) const
{
    return m_entries.at(m_filteredIndices.at(row));
}

QString FileIndexModel::pathAt(int filteredRow) const
{
    if (filteredRow < 0 || filteredRow >= m_filteredIndices.size()) {
        return QString();
    }
    return entryAtFilteredRow(filteredRow).path;
}

QStringList FileIndexModel::selectedPaths(const QVariantList& filteredRows) const
{
    QStringList paths;
    for (const QVariant& v : filteredRows) {
        const int row = v.toInt();
        const QString path = pathAt(row);
        if (!path.isEmpty()) {
            paths.append(path);
        }
    }
    return paths;
}

int FileIndexModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_filteredIndices.size();
}

int FileIndexModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant FileIndexModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_filteredIndices.size()) {
        return QVariant();
    }
    const FileIndexEntry& e = entryAtFilteredRow(index.row());
    const qint64 displaySize = m_useAllocatedSize ? e.allocatedSize : e.logicalSize;

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0: return e.path;
        case 1: return displaySize;
        case 2: return e.extension;
        case 3: return e.lastModified;
        default: break;
        }
        break;
    case PathRole: return e.path;
    case LogicalSizeRole: return e.logicalSize;
    case AllocatedSizeRole: return e.allocatedSize;
    case DisplaySizeRole: return displaySize;
    case ExtensionRole: return e.extension;
    case LastModifiedRole: return e.lastModified;
    default: break;
    }
    return QVariant();
}

QHash<int, QByteArray> FileIndexModel::roleNames() const
{
    return {
        {PathRole, "path"},
        {LogicalSizeRole, "logicalSize"},
        {AllocatedSizeRole, "allocatedSize"},
        {DisplaySizeRole, "displaySize"},
        {ExtensionRole, "extension"},
        {LastModifiedRole, "lastModified"}
    };
}
