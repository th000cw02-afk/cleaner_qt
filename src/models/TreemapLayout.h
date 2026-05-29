#ifndef TREEMAPLAYOUT_H
#define TREEMAPLAYOUT_H

#include <QObject>
#include <QRectF>
#include <QVector>
#include <QVariantList>
#include <memory>
#include "FolderNode.h"
#include "ExtensionColorMap.h"
#include "FileIndexEntry.h"

struct TreemapRect {
    QString path;
    QString name;
    qint64 size = 0;
    double sizePercent = 0.0;
    QRectF rect;
    QString color;
};

class TreemapLayout : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList rects READ rects NOTIFY rectsChanged)
    Q_PROPERTY(QString focusPath READ focusPath WRITE setFocusPath NOTIFY focusPathChanged)

public:
    explicit TreemapLayout(QObject* parent = nullptr);

    QVariantList rects() const;
    QString focusPath() const { return m_focusPath; }
    void setFocusPath(const QString& path);

    void buildFromNode(const std::shared_ptr<FolderNode>& root, qint64 totalSize,
                       const QString& focusPath,
                       const QVector<FileIndexEntry>& directFiles = {});
    Q_INVOKABLE int hitTest(qreal x, qreal y) const;
    Q_INVOKABLE QString pathAt(int index) const;

signals:
    void rectsChanged();
    void focusPathChanged();

private:
    void squarify(const QVector<std::shared_ptr<FolderNode>>& nodes,
                  qint64 totalSize,
                  const QRectF& area,
                  QVector<TreemapRect>& output);
    static QString colorForName(const QString& name);
    static QString colorForExtension(const QString& extension);

    QVariantList m_rectsVariant;
    QVector<TreemapRect> m_rects;
    QString m_focusPath;
    std::shared_ptr<FolderNode> m_root;
    qint64 m_totalSize = 0;
};

#endif // TREEMAPLAYOUT_H
