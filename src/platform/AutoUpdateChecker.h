#ifndef AUTOUPDATECHECKER_H
#define AUTOUPDATECHECKER_H

#include <QObject>
#include <QString>

class AutoUpdateChecker : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString latestVersion READ latestVersion NOTIFY checkFinished)
    Q_PROPERTY(QString releaseUrl READ releaseUrl NOTIFY checkFinished)

public:
    explicit AutoUpdateChecker(QObject* parent = nullptr);

    QString latestVersion() const { return m_latestVersion; }
    QString releaseUrl() const { return m_releaseUrl; }

    Q_INVOKABLE void checkForUpdates();

signals:
    void checkFinished(bool updateAvailable);

private:
    QString m_latestVersion;
    QString m_releaseUrl;
};

#endif // AUTOUPDATECHECKER_H
