#ifndef SERVICECONTROLLER_H
#define SERVICECONTROLLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QProcess>
#include <iostream>

class ServiceController : public QObject
{
    Q_OBJECT
public:
    enum ServiceStatus { Stopped, Running, Error, uninstalled };
    enum ServiceOperationType { Start, Stop, Restart, Create, PgSqlCreate };

    explicit ServiceController(QObject *parent = nullptr);

    ServiceStatus checkService(const QString &serviceName, int id);
    ServiceStatus checkServiceNoSignal(const QString &serviceName);
    bool startService(const QString &serviceName, int id);
    bool stopService(const QString &serviceName, int id);
    void restartService(const QString &serviceName, int id);
    bool createService(const QString &serviceName, const QString &binPath, int id);
    bool createPgSqlService(const QString &serviceName, const QString &binPath, const QString &dataPath, int id);
    void waitForFinished(const QString &serviceName, ServiceStatus needStatus, std::function<void()> callback);

signals:
    void serviceStatusChanged(const QString &serviceName, ServiceStatus status, int id);
    void finished(ServiceOperationType type, bool success, int id);

private:
    ServiceStatus parseWindowsServiceStatus(const QString &output);
    bool executeCommand(const QString &command, const QStringList &arguments = QStringList());
};

#endif // SERVICECONTROLLER_H
