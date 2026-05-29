#ifndef SCANMODULE_H
#define SCANMODULE_H

#include "../CleanupScanners.h"
#include <QString>
#include <QVariantList>
#include <functional>

class ScanModule {
public:
    virtual ~ScanModule() = default;
    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QVariantList run(const CleanupScanOptions& options) = 0;
};

class ScanModuleRegistry {
public:
    static ScanModuleRegistry& instance();
    QStringList moduleIds() const;
    QString displayNameFor(const QString& id) const;
    QVariantList runModule(const QString& id, const CleanupScanOptions& options);

private:
    ScanModuleRegistry();
    QVector<ScanModule*> m_modules;
};

#endif // SCANMODULE_H
