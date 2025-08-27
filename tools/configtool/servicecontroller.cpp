#include "servicecontroller.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>

ServiceController::ServiceController(QObject *parent) : QObject(parent) {}

ServiceController::ServiceStatus ServiceController::checkService(const QString &serviceName, int id) {
    QString output;
    QProcess process;
    process.start("sc", QStringList() << "query" << serviceName);
    if (process.waitForFinished(3000)) {
        output = QString::fromLocal8Bit(process.readAllStandardOutput());
        ServiceStatus status = parseWindowsServiceStatus(output);
        emit serviceStatusChanged(serviceName, status, id);
        return status;
    }
    emit serviceStatusChanged(serviceName, Error, id);
    return Error;
}

ServiceController::ServiceStatus ServiceController::checkServiceNoSignal(const QString &serviceName)
{
    QString output;
    QProcess process;
    process.start("sc", QStringList() << "query" << serviceName);
    if (process.waitForFinished(3000)) {
        output = QString::fromLocal8Bit(process.readAllStandardOutput());
        return parseWindowsServiceStatus(output);
    }
    return Error;
}

bool ServiceController::startService(const QString &serviceName, int id) {
    bool success = executeCommand("sc", QStringList() << "start" << serviceName);
    if(!success) {
        emit finished(Start, success, id);
        return success;
    }
    waitForFinished(serviceName, Running, [=](){
        emit finished(Start, success, id);
    });
    return success;
}

bool ServiceController::stopService(const QString &serviceName, int id) {
    bool success = executeCommand("sc", QStringList() << "stop" << serviceName);
    if(!success) {
        emit finished(Stop, success, id);
        return success;
    }
    waitForFinished(serviceName, Stopped, [=](){
        emit finished(Stop, success, id);
    });
    return success;
}

void ServiceController::restartService(const QString &serviceName, int id) {
    bool stopSuccess = executeCommand("sc", QStringList() << "stop" << serviceName);
    if(!stopSuccess) {
        emit finished(Restart, stopSuccess, id);
        return;
    }
    waitForFinished(serviceName, Stopped, [=](){
        bool success = executeCommand("sc", QStringList() << "start" << serviceName);
        if(!success) {
            emit finished(Restart, success, id);
            return;
        }
        waitForFinished(serviceName, Running, [=](){
            emit finished(Restart, success, id);
        });
    });
}

bool ServiceController::createService(const QString &name, const QString &binPath, int id) {
    QString createCmd = QString("\"%1_service.exe\" install")
                       .arg(binPath);
    QString startCmd = QString("sc start \"%1\"").arg(name);
    
    bool success = executeCommand(createCmd);
    QTimer::singleShot(1000, this, [=]() { 
        emit finished(Create, success, id);
    });
    QTimer::singleShot(500, this, [=](){
        bool m_success = success && executeCommand(startCmd);
        if(!m_success) {
            emit finished(Create, success, id);
            return;
        }
        waitForFinished(name, Running, [=](){
            emit finished(Create, success, id);
        });
    });
    return success;
}

bool ServiceController::createPgSqlService(const QString &name, const QString &binPath, const QString &dataPath, int id) {
    QString createCmd = QString("%1/pg_ctl register -D %2 -N %3 -S auto -U \"NT AUTHORITY/LocalService\"")
                       .arg(binPath, dataPath, name);
    QString startCmd = QString("sc start \"%1\"").arg(name);
    
    bool success = executeCommand(createCmd);
    QTimer::singleShot(500, this, [=](){
        bool m_success = success && executeCommand(startCmd);
        if(!m_success) {
            emit finished(PgSqlCreate, success, id);
            return;
        }
        waitForFinished(name, Running, [=](){
            emit finished(PgSqlCreate, success, id);
        });
    });
    return success;
}

ServiceController::ServiceStatus ServiceController::parseWindowsServiceStatus(const QString &output) {
    if (output.contains("RUNNING")) {
        return Running;
    } else if (output.contains("STOPPED")) {
        return Stopped;
    } else if (output.contains("1060")) {
        return uninstalled;
    }
    return Error;
}

bool ServiceController::executeCommand(const QString &command, const QStringList &arguments) {
    QProcess process;
    process.start(command, arguments);
    if (!process.waitForFinished(30000)) {
        qDebug() << command << "Execute Command error: " << process.errorString();
        return false;
    }
    return (process.exitCode() == 0);
}

void ServiceController::waitForFinished(const QString &serviceName, ServiceStatus needStatus, std::function<void()> callback)
{
    QTimer *delay = new QTimer;
    delay->setInterval(100);
    connect(delay, &QTimer::timeout, this, [=]() {
        if(checkServiceNoSignal(serviceName) == needStatus) {
            callback();
            delay->stop();
        }
    });
    delay->start();
}
