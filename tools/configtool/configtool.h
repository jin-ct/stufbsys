#ifndef CONFIGTOOL_H
#define CONFIGTOOL_H

#include <QWidget>
#include <QDir>
#include <iostream>
#include "servicecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ConfigTool;
}
QT_END_NAMESPACE

class ConfigTool : public QWidget
{
    Q_OBJECT

public:
    ConfigTool(QWidget *parent = nullptr);

    void setStatusToUi(ServiceController::ServiceStatus status, int id);
    void updateServiceStatus(const QString &serviceName, ServiceController::ServiceStatus status, int id);
    void onOperationFinished(ServiceController::ServiceOperationType type, bool success, int id);
    void showCert();
    void applyCer();

    ~ConfigTool();

signals:
    void checkService(const QString &serviceName, int id);
    void startService(const QString &serviceName, int id);
    void stopService(const QString &serviceName, int id);
    void restartService(const QString &serviceName, int id);
    void createService(const QString &serviceName, const QString &binPath, int id);
    void createPgSqlService(const QString &serviceName, const QString &binPath, const QString &dataPath, int id);

private:
    void resetPgSqlPassword(int id);
    void openFirewallWebPort();
    bool updateNginxConfig(const QString &filePath, std::function<void(QString &content)> editConfig);
    bool editNginxDomainConfig();
    bool openNginxSsl();
    void updateServerConfig(QString newDbPassword);
    bool executeCommand(const QString &command, const QStringList &arguments = QStringList());
    QString getRandomString(const int n);

    Ui::ConfigTool *ui;
    
    struct ServiceInfo {
        int id;
        QString name;
        QString binPath;
        ServiceController::ServiceStatus status;
        bool isCheckStatusRetrying = false;
    };
    QList<ServiceInfo> m_services;
    ServiceController *serviceController;
    QThread *serviceControllerThread;

    QDir *installPath;
    QString postgresPassword;
    QStringList domainNames;
    bool isOpenSsl = false;
    bool isUseAutoCert = false;
};
#endif // CONFIGTOOL_H
