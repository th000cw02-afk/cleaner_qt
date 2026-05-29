#include "ExtensionColorMap.h"
#include <QFileInfo>

const QStringList& ExtensionColorMap::palette()
{
    static const QStringList colors = {
        QStringLiteral("#42a5f5"), QStringLiteral("#66bb6a"), QStringLiteral("#ffa726"),
        QStringLiteral("#ab47bc"), QStringLiteral("#ef5350"), QStringLiteral("#26c6da"),
        QStringLiteral("#8d6e63"), QStringLiteral("#7e57c2"), QStringLiteral("#29b6f6"),
        QStringLiteral("#9ccc65"), QStringLiteral("#ff7043"), QStringLiteral("#5c6bc0")
    };
    return colors;
}

QString ExtensionColorMap::normalizeExtension(const QString& filePath)
{
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();
    if (ext.isEmpty()) {
        return QStringLiteral("(无扩展名)");
    }
    return QLatin1Char('.') + ext;
}

QString ExtensionColorMap::colorForExtension(const QString& extension)
{
    const QStringList& colors = palette();
    const uint hash = qHash(extension);
    return colors.at(hash % colors.size());
}
