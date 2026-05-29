#ifndef EXTENSIONCOLORMAP_H
#define EXTENSIONCOLORMAP_H

#include <QHash>
#include <QString>

class ExtensionColorMap {
public:
    static QString colorForExtension(const QString& extension);
    static QString normalizeExtension(const QString& filePath);

private:
    static const QStringList& palette();
};

#endif // EXTENSIONCOLORMAP_H
