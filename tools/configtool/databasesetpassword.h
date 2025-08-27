#ifndef DATABASESETPASSWORD_H
#define DATABASESETPASSWORD_H

#include <QThread>
#include <QDir>
#include "servicecontroller.h"

class DatabaseSetPassword : public QThread {
    Q_OBJECT
public:
    DatabaseSetPassword(QDir *installPathPtr, ServiceController *serviceControllerPtr, const QString &pgSqlServiceName, int id);

signals:
    void operFinished(bool success, QString newPassword = "");

protected:
    void run() override;
    QString getRandomString(const int n);
    bool executeCommand(const QString &command, const QStringList &arguments = QStringList());

    QDir *installPath;
    ServiceController *serviceControllerPtr;
    QString pgSqlServiceName;
    int id;
};

#endif // DATABASESETPASSWORD_H
