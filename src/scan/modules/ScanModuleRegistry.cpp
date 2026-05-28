#include "ScanModule.h"
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QStorageInfo>
#include <algorithm>

namespace {

class EmptyDirsModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("emptyDirs"); }
    QString displayName() const override { return QStringLiteral("空目录"); }
    QVariantList run(const CleanupScanOptions& options) override {
        return CleanupScanners::findEmptyDirectories(options);
    }
};

class DuplicatesModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("duplicates"); }
    QString displayName() const override { return QStringLiteral("重复文件"); }
    QVariantList run(const CleanupScanOptions& options) override {
        return CleanupScanners::findDuplicateCandidates(options);
    }
};

class BigFilesModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("bigFiles"); }
    QString displayName() const override { return QStringLiteral("大文件"); }
    QVariantList run(const CleanupScanOptions& options) override {
        QVariantList results;
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& file : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                if (file.size() >= options.minFileSize && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = file.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("big_file");
                    map[QStringLiteral("size")] = file.size();
                    results.append(map);
                }
            }
            for (const QFileInfo& sub : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                walk(sub.absoluteFilePath());
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class TempFilesModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("tempFiles"); }
    QString displayName() const override { return QStringLiteral("临时文件"); }
    QVariantList run(const CleanupScanOptions& options) override {
        static const QStringList patterns = {QStringLiteral(".tmp"), QStringLiteral(".temp"),
                                             QStringLiteral("~")};
        QVariantList results;
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& file : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                const QString name = file.fileName().toLower();
                bool match = name.contains(QStringLiteral("temp")) || name.endsWith(QStringLiteral(".tmp"));
                for (const QString& p : patterns) {
                    if (name.endsWith(p)) {
                        match = true;
                    }
                }
                if (match && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = file.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("temp_file");
                    results.append(map);
                }
            }
            for (const QFileInfo& sub : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                walk(sub.absoluteFilePath());
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class ZeroLengthModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("zeroLength"); }
    QString displayName() const override { return QStringLiteral("零字节文件"); }
    QVariantList run(const CleanupScanOptions& options) override {
        QVariantList results;
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& file : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                if (file.size() == 0 && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = file.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("zero_length");
                    results.append(map);
                }
            }
            for (const QFileInfo& sub : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                walk(sub.absoluteFilePath());
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class OldFilesModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("oldFiles"); }
    QString displayName() const override { return QStringLiteral("长期未修改"); }
    QVariantList run(const CleanupScanOptions& options) override {
        QVariantList results;
        const QDateTime threshold = QDateTime::currentDateTime().addDays(-365);
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& file : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                if (file.lastModified() < threshold && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = file.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("old_file");
                    map[QStringLiteral("lastModified")] = file.lastModified();
                    results.append(map);
                }
            }
            for (const QFileInfo& sub : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                walk(sub.absoluteFilePath());
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class LongPathsModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("longPaths"); }
    QString displayName() const override { return QStringLiteral("长路径"); }
    QVariantList run(const CleanupScanOptions& options) override {
        QVariantList results;
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& entry : dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (entry.absoluteFilePath().length() > 260 && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = entry.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("long_path");
                    results.append(map);
                }
                if (entry.isDir()) {
                    walk(entry.absoluteFilePath());
                }
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class LockedFilesModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("lockedFiles"); }
    QString displayName() const override { return QStringLiteral("被锁定文件"); }
    QVariantList run(const CleanupScanOptions& options) override {
        QVariantList results;
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& file : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                QFile f(file.absoluteFilePath());
                if (!f.open(QIODevice::ReadWrite) && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = file.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("locked_file");
                    results.append(map);
                }
            }
            for (const QFileInfo& sub : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                walk(sub.absoluteFilePath());
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class BrokenLinksModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("brokenLinks"); }
    QString displayName() const override { return QStringLiteral("损坏链接"); }
    QVariantList run(const CleanupScanOptions& options) override {
        Q_UNUSED(options);
        return {};
    }
};

class BrokenShortcutsModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("brokenShortcuts"); }
    QString displayName() const override { return QStringLiteral("损坏快捷方式"); }
    QVariantList run(const CleanupScanOptions& options) override {
        QVariantList results;
        std::function<void(const QString&)> walk;
        walk = [&](const QString& path) {
            QDir dir(path);
            for (const QFileInfo& file : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                if (file.suffix().compare(QStringLiteral("lnk"), Qt::CaseInsensitive) == 0
                    && results.size() < options.maxResults) {
                    QVariantMap map;
                    map[QStringLiteral("path")] = file.absoluteFilePath();
                    map[QStringLiteral("type")] = QStringLiteral("broken_shortcut");
                    results.append(map);
                }
            }
            for (const QFileInfo& sub : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                walk(sub.absoluteFilePath());
            }
        };
        walk(options.rootPath);
        return results;
    }
};

class DupArchivesModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("dupArchives"); }
    QString displayName() const override { return QStringLiteral("压缩包旁路"); }
    QVariantList run(const CleanupScanOptions& options) override {
        Q_UNUSED(options);
        return {};
    }
};

class OrphanedAppDataModule : public ScanModule {
public:
    QString id() const override { return QStringLiteral("orphanedAppData"); }
    QString displayName() const override { return QStringLiteral("卸载残留"); }
    QVariantList run(const CleanupScanOptions& options) override {
        Q_UNUSED(options);
        return {};
    }
};

} // namespace

ScanModuleRegistry& ScanModuleRegistry::instance()
{
    static ScanModuleRegistry registry;
    return registry;
}

ScanModuleRegistry::ScanModuleRegistry()
{
    m_modules = {
        new EmptyDirsModule(),
        new DuplicatesModule(),
        new BigFilesModule(),
        new TempFilesModule(),
        new ZeroLengthModule(),
        new OldFilesModule(),
        new BrokenLinksModule(),
        new BrokenShortcutsModule(),
        new LongPathsModule(),
        new LockedFilesModule(),
        new DupArchivesModule(),
        new OrphanedAppDataModule()
    };
}

QStringList ScanModuleRegistry::moduleIds() const
{
    QStringList ids;
    for (ScanModule* module : m_modules) {
        ids.append(module->id());
    }
    return ids;
}

QString ScanModuleRegistry::displayNameFor(const QString& id) const
{
    for (ScanModule* module : m_modules) {
        if (module->id() == id) {
            return module->displayName();
        }
    }
    return id;
}

QVariantList ScanModuleRegistry::runModule(const QString& id, const CleanupScanOptions& options)
{
    for (ScanModule* module : m_modules) {
        if (module->id() == id) {
            return module->run(options);
        }
    }
    return {};
}
