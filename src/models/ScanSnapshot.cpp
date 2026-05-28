#include "ScanSnapshot.h"
#include "FileIndexEntry.h"
#include "FolderNode.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

namespace {

QJsonObject nodeToJson(const std::shared_ptr<FolderNode>& node)
{
    QJsonObject obj;
    obj[QStringLiteral("path")] = node->path;
    obj[QStringLiteral("name")] = node->name;
    obj[QStringLiteral("size")] = node->size;
    obj[QStringLiteral("allocatedSize")] = node->allocatedSize;
    obj[QStringLiteral("fileCount")] = node->fileCount;
    obj[QStringLiteral("dirCount")] = node->dirCount;
    if (node->lastModified.isValid()) {
        obj[QStringLiteral("lastModified")] = node->lastModified.toString(Qt::ISODate);
    }
    QJsonArray children;
    for (const auto& child : node->children) {
        children.append(nodeToJson(child));
    }
    obj[QStringLiteral("children")] = children;
    return obj;
}

std::shared_ptr<FolderNode> nodeFromJson(const QJsonObject& obj, FolderNode* parent)
{
    auto node = std::make_shared<FolderNode>(obj.value(QStringLiteral("path")).toString(), parent);
    node->name = obj.value(QStringLiteral("name")).toString();
    node->size = obj.value(QStringLiteral("size")).toVariant().toLongLong();
    node->allocatedSize = obj.value(QStringLiteral("allocatedSize")).toVariant().toLongLong();
    node->fileCount = obj.value(QStringLiteral("fileCount")).toInt();
    node->dirCount = obj.value(QStringLiteral("dirCount")).toInt();
    const QString mtime = obj.value(QStringLiteral("lastModified")).toString();
    if (!mtime.isEmpty()) {
        node->lastModified = QDateTime::fromString(mtime, Qt::ISODate);
    }
    const QJsonArray children = obj.value(QStringLiteral("children")).toArray();
    for (const QJsonValue& value : children) {
        node->addChild(nodeFromJson(value.toObject(), node.get()));
    }
    return node;
}

QJsonArray fileIndexToJson(const QVector<FileIndexEntry>& entries)
{
    QJsonArray array;
    for (const FileIndexEntry& entry : entries) {
        QJsonObject obj;
        obj[QStringLiteral("path")] = entry.path;
        obj[QStringLiteral("logicalSize")] = entry.logicalSize;
        obj[QStringLiteral("allocatedSize")] = entry.allocatedSize;
        obj[QStringLiteral("extension")] = entry.extension;
        obj[QStringLiteral("fileIndexKey")] = QString::number(entry.fileIndexKey);
        if (entry.lastModified.isValid()) {
            obj[QStringLiteral("lastModified")] = entry.lastModified.toString(Qt::ISODate);
        }
        array.append(obj);
    }
    return array;
}

QVector<FileIndexEntry> fileIndexFromJson(const QJsonArray& array)
{
    QVector<FileIndexEntry> entries;
    entries.reserve(array.size());
    for (const QJsonValue& value : array) {
        const QJsonObject obj = value.toObject();
        FileIndexEntry entry;
        entry.path = obj.value(QStringLiteral("path")).toString();
        entry.logicalSize = obj.value(QStringLiteral("logicalSize")).toVariant().toLongLong();
        entry.allocatedSize = obj.value(QStringLiteral("allocatedSize")).toVariant().toLongLong();
        entry.extension = obj.value(QStringLiteral("extension")).toString();
        entry.fileIndexKey = obj.value(QStringLiteral("fileIndexKey")).toString().toULongLong();
        const QString mtime = obj.value(QStringLiteral("lastModified")).toString();
        if (!mtime.isEmpty()) {
            entry.lastModified = QDateTime::fromString(mtime, Qt::ISODate);
        }
        entries.append(entry);
    }
    return entries;
}

} // namespace

bool ScanSnapshot::save(const QString& filePath, const ScanResult& result, QString* errorOut)
{
    if (!result.root) {
        if (errorOut) {
            *errorOut = QStringLiteral("无扫描数据可保存");
        }
        return false;
    }

    QJsonObject root;
    root[QStringLiteral("version")] = 1;
    root[QStringLiteral("scanPath")] = result.scanPath;
    root[QStringLiteral("totalSize")] = result.totalSize;
    root[QStringLiteral("totalAllocatedSize")] = result.totalAllocatedSize;
    root[QStringLiteral("totalFiles")] = result.totalFiles;
    root[QStringLiteral("directoriesScanned")] = result.directoriesScanned;
    root[QStringLiteral("usedMft")] = result.usedMft;
    root[QStringLiteral("tree")] = nodeToJson(result.root);
    root[QStringLiteral("fileIndex")] = fileIndexToJson(result.fileIndex);
    root[QStringLiteral("extensionStats")] = QJsonArray::fromVariantList(result.extensionStats);
    root[QStringLiteral("duplicateGroups")] = QJsonArray::fromVariantList(result.duplicateGroups);

    QJsonArray topFiles;
    for (const FileInfo& info : result.topFiles) {
        QJsonObject obj;
        obj[QStringLiteral("path")] = info.path;
        obj[QStringLiteral("size")] = info.size;
        obj[QStringLiteral("lastModified")] = info.lastModified.toString(Qt::ISODate);
        topFiles.append(obj);
    }
    root[QStringLiteral("topFiles")] = topFiles;

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorOut) {
            *errorOut = file.errorString();
        }
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    if (!file.commit()) {
        if (errorOut) {
            *errorOut = file.errorString();
        }
        return false;
    }
    return true;
}

bool ScanSnapshot::load(const QString& filePath, ScanResult* resultOut, QString* errorOut)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorOut) {
            *errorOut = file.errorString();
        }
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        if (errorOut) {
            *errorOut = QStringLiteral("快照格式无效");
        }
        return false;
    }

    const QJsonObject root = doc.object();
    ScanResult result;
    result.scanPath = root.value(QStringLiteral("scanPath")).toString();
    result.totalSize = root.value(QStringLiteral("totalSize")).toVariant().toLongLong();
    result.totalAllocatedSize = root.value(QStringLiteral("totalAllocatedSize")).toVariant().toLongLong();
    result.totalFiles = root.value(QStringLiteral("totalFiles")).toInt();
    result.directoriesScanned = root.value(QStringLiteral("directoriesScanned")).toInt();
    result.usedMft = root.value(QStringLiteral("usedMft")).toBool();
    result.root = nodeFromJson(root.value(QStringLiteral("tree")).toObject(), nullptr);
    result.fileIndex = fileIndexFromJson(root.value(QStringLiteral("fileIndex")).toArray());
    result.extensionStats = root.value(QStringLiteral("extensionStats")).toArray().toVariantList();
    result.duplicateGroups = root.value(QStringLiteral("duplicateGroups")).toArray().toVariantList();

    const QJsonArray topFiles = root.value(QStringLiteral("topFiles")).toArray();
    for (const QJsonValue& value : topFiles) {
        const QJsonObject obj = value.toObject();
        FileInfo info(obj.value(QStringLiteral("path")).toString(),
                      obj.value(QStringLiteral("size")).toVariant().toLongLong(),
                      QDateTime::fromString(obj.value(QStringLiteral("lastModified")).toString(), Qt::ISODate),
                      false);
        result.topFiles.append(info);
    }

    *resultOut = result;
    return true;
}
