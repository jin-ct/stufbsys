#ifndef INSTALLER_H
#define INSTALLER_H

#define PACKAGE_WEB_VERSION "1.0"
#define PACKAGE_BACKEND_VERSION "1.0"
#define PACKAGE_CONFIGTOOL_VERSION "1.0"

#include <QWidget>
#include <QMouseEvent>
#include <QDir>
#include "servicecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class installer;
}
QT_END_NAMESPACE

class installer : public QWidget
{
    Q_OBJECT

public:
    installer(QWidget *parent = nullptr);
    ~installer();

    void choseInstallPath();
    void startInstall();
    void startInstallWithStopServices();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initDefaultInstallPath();
    void addLinkToDesktop(const QString& filename, const QString& name);
    bool clearDir(QString path);

    struct ServiceInfo {
        int id;
        QString name;
        QString nickname;
        bool isClosed = false;
    };
    QList<ServiceInfo> m_services;
    ServiceController *serviceController;

    Ui::installer *ui;
    bool m_dragging = false;
    QPoint m_dragPosition;

    QDir installPath;
    QString curWebVersion;
    QString curbackendVersion;
    QString curConfigToolVersion;
    bool isNewInstall = false;
    bool isClearAll = false;
    QStringList ignorePaths;
};
#endif // INSTALLER_H
