#ifndef FOLDERTREEMODEL_H
#define FOLDERTREEMODEL_H

#include <QAbstractItemModel>
#include <memory>
#include "FolderNode.h"

class FolderTreeModel : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(qint64 rootSize READ rootSize NOTIFY rootChanged)

public:
    enum Roles {
        PathRole = Qt::UserRole + 1,
        SizeRole,
        FileCountRole,
        DirCountRole,
        SizePercentRole,
        AllocatedSizeRole,
        LastModifiedRole,
        NameRole
    };

    explicit FolderTreeModel(QObject* parent = nullptr);

    qint64 rootSize() const { return m_rootSize; }

    void setTree(std::shared_ptr<FolderNode> root, qint64 totalSize);
    void clear();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void rootChanged();

private:
    FolderNode* nodeFromIndex(const QModelIndex& index) const;

    std::shared_ptr<FolderNode> m_root;
    qint64 m_rootSize = 0;
};

#endif // FOLDERTREEMODEL_H
