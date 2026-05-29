#include "ReportExporter.h"
#include <QFile>
#include <QStringConverter>
#include <QTextStream>

static QString escapeCsv(const QString& value)
{
    QString v = value;
    if (v.contains(QLatin1Char('"')) || v.contains(QLatin1Char(','))) {
        v.replace(QLatin1Char('"'), QStringLiteral("\"\""));
        return QStringLiteral("\"%1\"").arg(v);
    }
    return v;
}

bool ReportExporter::exportCsv(const QString& filePath, const std::shared_ptr<FolderNode>& root,
                               const QVector<FileInfo>& topFiles, QString* errorOut)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorOut) {
            *errorOut = file.errorString();
        }
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "type,path,size_bytes,file_count\n";
    if (root) {
        writeFolderRows(out, root, 0);
    }
    out << "\n# top files\npath,size_bytes\n";
    for (const FileInfo& info : topFiles) {
        out << escapeCsv(info.path) << ',' << info.size << '\n';
    }
    return true;
}

bool ReportExporter::exportHtml(const QString& filePath, const std::shared_ptr<FolderNode>& root,
                                  const QVector<FileInfo>& topFiles, qint64 totalSize, QString* errorOut)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorOut) {
            *errorOut = file.errorString();
        }
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Disk Analysis Report</title>"
           "<style>body{font-family:sans-serif}table{border-collapse:collapse;width:100%}"
           "td,th{border:1px solid #ccc;padding:6px}</style></head><body>";
    out << "<h1>磁盘空间分析报告</h1>";
    out << "<p>总大小: " << totalSize << " 字节</p>";
    out << "<h2>目录树</h2><table><tr><th>路径</th><th>大小(字节)</th><th>文件数</th></tr>";
    if (root) {
        writeFolderRowsHtml(out, root, 0);
    }
    out << "</table><h2>Top 大文件</h2><table><tr><th>路径</th><th>大小(字节)</th></tr>";
    for (const FileInfo& info : topFiles) {
        out << "<tr><td>" << info.path.toHtmlEscaped() << "</td><td>" << info.size << "</td></tr>";
    }
    out << "</table></body></html>";
    return true;
}

void ReportExporter::writeFolderRows(QTextStream& out, const std::shared_ptr<FolderNode>& node, int depth)
{
    if (!node) {
        return;
    }
    out << "folder," << escapeCsv(node->path) << ',' << node->size << ',' << node->fileCount << '\n';
    for (const auto& child : node->children) {
        writeFolderRows(out, child, depth + 1);
    }
}

void ReportExporter::writeFolderRowsHtml(QTextStream& out, const std::shared_ptr<FolderNode>& node, int depth)
{
    if (!node) {
        return;
    }
    out << "<tr><td style=\"padding-left:" << (depth * 16) << "px\">" << node->path.toHtmlEscaped()
        << "</td><td>" << node->size << "</td><td>" << node->fileCount << "</td></tr>";
    for (const auto& child : node->children) {
        writeFolderRowsHtml(out, child, depth + 1);
    }
}
