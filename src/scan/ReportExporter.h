#ifndef REPORTEXPORTER_H
#define REPORTEXPORTER_H

#include <QString>
#include <memory>
#include "../models/FolderNode.h"
#include "../FileInfo.h"

class ReportExporter {
public:
    static bool exportCsv(const QString& filePath, const std::shared_ptr<FolderNode>& root,
                          const QVector<FileInfo>& topFiles, QString* errorOut = nullptr);
    static bool exportHtml(const QString& filePath, const std::shared_ptr<FolderNode>& root,
                           const QVector<FileInfo>& topFiles, qint64 totalSize, QString* errorOut = nullptr);

private:
    static void writeFolderRows(QTextStream& out, const std::shared_ptr<FolderNode>& node, int depth);
    static void writeFolderRowsHtml(QTextStream& out, const std::shared_ptr<FolderNode>& node, int depth);
};

#endif // REPORTEXPORTER_H
