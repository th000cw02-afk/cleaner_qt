#ifndef FILEWATCHERSERVICE_WIN_H
#define FILEWATCHERSERVICE_WIN_H

#include <QObject>
#include <QStringList>

class FileWatcherService : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool watching READ watching NOTIFY watchingChanged)
    Q_PROPERTY(QString watchPath READ watchPath NOTIFY watchPathChanged)
    Q_PROPERTY(QStringList log READ log NOTIFY logChanged)

public:
    explicit FileWatcherService(QObject* parent = nullptr);
    ~FileWatcherService() override;

    bool watching() const { return m_watching; }
    QString watchPath() const { return m_watchPath; }
    QStringList log() const { return m_log; }

    Q_INVOKABLE void startWatch(const QString& path);
    Q_INVOKABLE void stopWatch();
    Q_INVOKABLE void clearLog();

signals:
    void watchingChanged();
    void watchPathChanged();
    void logChanged();
    void fileChanged(const QString& path, const QString& action);

private:
    bool m_watching = false;
    QString m_watchPath;
    QStringList m_log;
};

#endif // FILEWATCHERSERVICE_WIN_H
