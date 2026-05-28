#include "AutoUpdateChecker.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

AutoUpdateChecker::AutoUpdateChecker(QObject* parent)
    : QObject(parent)
{
}

void AutoUpdateChecker::checkForUpdates()
{
    auto* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(
        QUrl(QStringLiteral("https://api.github.com/repos/cleaner_qt/cleaner_qt/releases/latest"))));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            const QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
            m_latestVersion = obj.value(QStringLiteral("tag_name")).toString();
            m_releaseUrl = obj.value(QStringLiteral("html_url")).toString();
        }
        reply->deleteLater();
        const bool available = !m_latestVersion.isEmpty()
            && m_latestVersion != QCoreApplication::applicationVersion();
        emit checkFinished(available);
    });
}
