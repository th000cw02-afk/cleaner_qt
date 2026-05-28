#include "CleanupScanners.h"
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QVariantMap>
#include <functional>
#include <algorithm>

static bool isDirEmpty(const QString& path)
{
    QDir dir(path);
    return dir.exists() && dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty();
}

QVariantList CleanupScanners::findEmptyDirectories(const CleanupScanOptions& options)
{
    QVariantList results;
    QDir root(options.rootPath);
    if (!root.exists()) {
        return results;
    }

    std::function<void(const QString&)> walk;
    walk = [&](const QString& path) {
        QDir dir(path);
        const QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            walk(entry.absoluteFilePath());
        }
        if (isDirEmpty(path) && path != options.rootPath) {
            if (results.size() >= options.maxResults) {
                return;
            }
            QVariantMap map;
            map[QStringLiteral("path")] = path;
            map[QStringLiteral("type")] = QStringLiteral("empty_dir");
            results.append(map);
        }
    };

    walk(QDir::toNativeSeparators(QFileInfo(options.rootPath).absoluteFilePath()));
    return results;
}

QVariantList CleanupScanners::findDuplicateCandidates(const CleanupScanOptions& options)
{
    QVariantList results;
    QHash<qint64, QVector<QFileInfo>> bySize;

    std::function<void(const QString&)> walk;
    walk = [&](const QString& path) {
        QDir dir(path);
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        for (const QFileInfo& file : dir.entryInfoList()) {
            if (file.size() < options.minFileSize) {
                continue;
            }
            bySize[file.size()].append(file);
        }
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
        for (const QFileInfo& sub : dir.entryInfoList()) {
            walk(sub.absoluteFilePath());
        }
    };

    walk(QDir::toNativeSeparators(QFileInfo(options.rootPath).absoluteFilePath()));

    for (auto it = bySize.constBegin(); it != bySize.constEnd(); ++it) {
        if (it.value().size() < 2) {
            continue;
        }
        QHash<QByteArray, QVector<QFileInfo>> byPrefix;
        for (const QFileInfo& file : it.value()) {
            QFile f(file.absoluteFilePath());
            if (!f.open(QIODevice::ReadOnly)) {
                continue;
            }
            const QByteArray prefix = f.read(4096);
            byPrefix[prefix].append(file);
        }
        for (auto pit = byPrefix.constBegin(); pit != byPrefix.constEnd(); ++pit) {
            if (pit.value().size() < 2) {
                continue;
            }
            QHash<QByteArray, QVector<QFileInfo>> byFull;
            for (const QFileInfo& file : pit.value()) {
                QFile f(file.absoluteFilePath());
                if (!f.open(QIODevice::ReadOnly)) {
                    continue;
                }
                QCryptographicHash hash(QCryptographicHash::Sha256);
                if (hash.addData(&f)) {
                    byFull[hash.result()].append(file);
                }
            }
            for (auto fit = byFull.constBegin(); fit != byFull.constEnd(); ++fit) {
                if (fit.value().size() < 2) {
                    continue;
                }
                if (results.size() >= options.maxResults) {
                    return results;
                }
                QVariantMap group;
                group[QStringLiteral("type")] = QStringLiteral("duplicate_group");
                group[QStringLiteral("size")] = fit.value().first().size();
                QVariantList paths;
                for (const QFileInfo& file : fit.value()) {
                    paths.append(file.absoluteFilePath());
                }
                group[QStringLiteral("paths")] = paths;
                results.append(group);
            }
        }
    }
    return results;
}
