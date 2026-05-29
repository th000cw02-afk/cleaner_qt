#include "AppLogger.h"

#include <QMessageLogContext>
#include <QString>

#include <cstring>

namespace {

void qtMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (!AppLogger::isInitialized()) {
        return;
    }

    QString formatted = msg;
    if (context.category && qstrlen(context.category) > 0) {
        formatted = QStringLiteral("[%1] %2").arg(QString::fromUtf8(context.category), msg);
    }
    if (context.file && context.line > 0) {
        formatted += QStringLiteral(" (%1:%2)").arg(QString::fromUtf8(context.file)).arg(context.line);
    }

    const std::string text = formatted.toStdString();
    switch (type) {
    case QtDebugMsg:
        AppLogger::log(AppLogger::Level::Debug, text);
        break;
    case QtInfoMsg:
        AppLogger::log(AppLogger::Level::Info, text);
        break;
    case QtWarningMsg:
        AppLogger::log(AppLogger::Level::Warn, text);
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        AppLogger::log(AppLogger::Level::Error, text);
        break;
    }
}

} // namespace

void installQtMessageHandler()
{
    qInstallMessageHandler(qtMessageHandler);
}
