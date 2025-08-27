#include "databasesetpassword.h"
#include <QRandomGenerator>
#include <QProcess>
#include <QSettings>
#include <QDebug>

void DatabaseSetPassword::run()
{
    // 检查服务是否运行
    if(serviceControllerPtr->checkServiceNoSignal(pgSqlServiceName) != ServiceController::Running){
        qWarning() << "PostgreSQL service is not running, try to start";
        if(!serviceControllerPtr->startService(pgSqlServiceName, id)){
            qWarning() << "PostgreSQL service is not running, and start error";
            emit operFinished(false);
            return;
        }
    }

    QString password = getRandomString(16);
    QStringList arguments = QStringList() << "--username" << "postgres" << "--command" << QString("ALTER USER postgres WITH PASSWORD '%1';").arg(password);
    bool success = executeCommand(QString("%1/vendors/pgsql/bin/psql").arg(installPath->absolutePath()), arguments);

    if(!success){
        emit operFinished(success);
        return;
    }

    // 关闭服务
    if(!serviceControllerPtr->stopService(pgSqlServiceName, id)){
        qWarning() << "Failed to stop PostgreSQL service";
    }

    // 修改 pg_hba.conf
    QString pg_hba_conf = QString("%1/db_data/pg_hba.conf").arg(installPath->absolutePath());
    QFile file(pg_hba_conf);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString content = file.readAll();
        content.replace("trust", "scram-sha-256");
        file.seek(0);
        file.write(content.toUtf8());
        file.close();
    }else{
        qWarning() << "Failed to update pg_hba.conf";
    }

    serviceControllerPtr->waitForFinished(pgSqlServiceName, ServiceController::Stopped, [=]() {
        if(!serviceControllerPtr->startService(pgSqlServiceName, id)){
            qWarning() << "Failed to start PostgreSQL service";
        }
    });

    // 保存postgres密码
    QSettings settings(QString("%1/bin/conf/config.ini").arg(installPath->absolutePath()), QSettings::IniFormat);
    settings.setValue("postgres_password", QVariant(password));
    settings.sync();

    emit operFinished(success, password);
}

DatabaseSetPassword::DatabaseSetPassword(QDir *installPathPtr, ServiceController *serviceControllerPtr, const QString &pgSqlServiceName, int id)
{
    this->installPath = installPathPtr;
    this->serviceControllerPtr = serviceControllerPtr;
    this->pgSqlServiceName = pgSqlServiceName;
    this->id = id;
}

QString DatabaseSetPassword::getRandomString(const int n)
{
    QString str;
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < n; ++i) {
        int index = QRandomGenerator::global()->bounded(sizeof(alphanum) - 1);
        str.append(alphanum[index]);
    }
    return str;
}

bool DatabaseSetPassword::executeCommand(const QString &command, const QStringList &arguments) {
    QProcess process;
    process.start(command, arguments);
    if (!process.waitForFinished(30000)) {
        qDebug() << command << "Execute Command error: " << process.errorString();
        return false;
    }
    return (process.exitCode() == 0);
}
