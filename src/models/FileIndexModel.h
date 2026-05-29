#ifndef FILEINDEXMODEL_H
#define FILEINDEXMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QRegularExpression>
#include "FileIndexEntry.h"

class FileIndexModel : public QAbstractTableModel {
    Q_OBJECT
    Q_PROPERTY(int filteredRowCount READ filteredRowCount NOTIFY filterChanged)
    Q_PROPERTY(bool useAllocatedSize READ useAllocatedSize WRITE setUseAllocatedSize NOTIFY useAllocatedSizeChanged)

public:
    enum Roles {
        PathRole = Qt::UserRole + 1,
        LogicalSizeRole,
        AllocatedSizeRole,
        DisplaySizeRole,
        ExtensionRole,
        LastModifiedRole
    };

    explicit FileIndexModel(QObject* parent = nullptr);

    bool useAllocatedSize() const { return m_useAllocatedSize; }
    void setUseAllocatedSize(bool value);
    int filteredRowCount() const { return m_filteredIndices.size(); }

    void setEntries(QVector<FileIndexEntry> entries);
    void clear();
    void setFilterText(const QString& text);
    void setRegexFilter(const QString& pattern, bool enabled);
    void setExtensionFilter(const QString& extension);

    Q_INVOKABLE QString pathAt(int filteredRow) const;
    Q_INVOKABLE QStringList selectedPaths(const QVariantList& filteredRows) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void filterChanged();
    void useAllocatedSizeChanged();

private:
    void rebuildFilter();
    const FileIndexEntry& entryAtFilteredRow(int row) const;

    QVector<FileIndexEntry> m_entries;
    QVector<int> m_filteredIndices;
    QString m_filterText;
    QString m_extensionFilter;
    QRegularExpression m_regexFilter;
    bool m_regexEnabled = false;
    bool m_useAllocatedSize = false;
};

#endif // FILEINDEXMODEL_H
