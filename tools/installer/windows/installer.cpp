#include "installer.h"
#include "./ui_installer.h"
#include <QFileDialog>
#include <QMessageBox>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <QFile>
#include <QStandardPaths>
#include <QProcess>
#include <QSettings>
#include <QThread>

installer::installer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::installer)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // 初始化服务信息
    m_services = {
        {
            0,
            "StuFBSysPostgreSQL",
            "PostgreSQL"
        },
        {
            1,
            "StuFBSysNginx",
            "Nginx"
        },
        {
            2,
            "StuFBSysBackend",
            "后端"
        }
    };
    serviceController = new ServiceController(this);

    initDefaultInstallPath();
    connect(ui->installPathChoseBtn, &QPushButton::clicked, this, &installer::choseInstallPath);
    connect(ui->startBtn, &QPushButton::clicked, this, [=](){
        installPath = QDir(ui->installPathEditer->text());
        if (QDir(installPath.filePath("db_data")).exists()) {
            isNewInstall = false;
            QMessageBox::StandardButton btn =
                QMessageBox::question(this, "安装更新", "检测到系统已安装，你是需要更新版本吗？\n按“Yes”则更新版本\n按“No”则重新覆盖安装");
            if (btn == QMessageBox::Yes) {
                // 加载配置文件
                QSettings settings(installPath.filePath("bin/conf/version.ini"), QSettings::IniFormat);
                curWebVersion = settings.value("web_version").toString();
                curbackendVersion = settings.value("backend_version").toString();
                curConfigToolVersion = settings.value("configtool_version").toString();

                // 跳过的目录
                ignorePaths.append(installPath.filePath("db_data"));
                ignorePaths.append(installPath.filePath("vendors"));
                ignorePaths.append(installPath.filePath("bin/log"));
                ignorePaths.append(installPath.filePath("server/log"));
                if (curWebVersion == PACKAGE_WEB_VERSION) ignorePaths.append(installPath.filePath("web"));
                if (curbackendVersion == PACKAGE_BACKEND_VERSION) ignorePaths.append(installPath.filePath("server"));
                if (curConfigToolVersion == PACKAGE_CONFIGTOOL_VERSION) ignorePaths.append(installPath.filePath("bin"));
                startInstallWithStopServices();
            } else if (btn == QMessageBox::No) {
                QMessageBox::StandardButton confirmBtn = QMessageBox::question(this, "注意", "覆盖安装将会丢失所有数据，你确定要继续吗？");
                if (confirmBtn == QMessageBox::Yes) {
                    isClearAll = true;
                    startInstallWithStopServices();
                }
            }
            return;
        }
        if (!installPath.exists()) {
            if (!QDir().mkpath(installPath.absolutePath())) {
                QMessageBox::warning(this, "创建文件夹失败", "创建文件夹失败，请尝试以管理员身份运行！");
                this->close();
                return;
            }
        }
        isNewInstall = true;
        startInstall();
    });
}

installer::~installer()
{
    delete ui;
}

void installer::choseInstallPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择安装路径", installPath.absolutePath());
    if (!dir.isEmpty()) {
        installPath = QDir(dir + "/stufbsys");
        ui->installPathEditer->setText(installPath.absolutePath());
    }
}

void installer::startInstall()
{
    if (!isNewInstall) {
        if (!m_services[0].isClosed ||
            !m_services[1].isClosed ||
            !m_services[2].isClosed
        ) {
            return;
        }

        if (isClearAll) {
            ui->pages->setCurrentIndex(1);  // 跳转至安装中页面
            ui->installTip->setText("正在清空安装目录");
            if (!clearDir(installPath.absolutePath())) {
                QMessageBox::warning(this, "清空安装目录失败", "清空安装目录失败，请尝试以管理员身份运行（如需手动清空目录，请确保所有服务已停止）");
                this->close();
            }
        }
    }

    ui->pages->setCurrentIndex(1);  // 跳转至安装中页面
    ui->installTip->setText("正在安装");
    ui->installProgress->setValue(0);

    // 将资源文件复制到临时目录
    QString tempZipPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/stufbsys.zip";
    QFile resFile(":/package/stufbsys.zip");
    if (!resFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "资源读取失败", "无法读取安装包资源！");
        return;
    }
    QFile tempZip(tempZipPath);
    if (!tempZip.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "临时文件创建失败", "无法创建临时安装包文件！");
        return;
    }
    tempZip.write(resFile.readAll());
    tempZip.close();
    resFile.close();

    // 用 QuaZip 解压临时文件
    QuaZip zip(tempZipPath);
    if (!zip.open(QuaZip::mdUnzip)) {
        QMessageBox::warning(this, "解压失败", "无法打开安装包，请重试！");
        return;
    }

    QList<QuaZipFileInfo> entries;
    QuaZipFileInfo info;
    QuaZipFile zipFile(&zip);
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        zip.getCurrentFileInfo(&info);
        entries.append(info);
    }

    int totalFiles = entries.size();
    int currentFile = 0;

    for (const auto &entry : entries) {
        QString outPath = installPath.filePath(entry.name);
        if (entry.name.endsWith('/')) {
            QDir().mkpath(outPath);
        } else {
            if (zip.setCurrentFile(entry.name)) {
                QFile outFile(outPath);
                QFileInfo outInfo(outFile);

                bool isNotIgnore = true;
                if (!isNewInstall) {
                    for (auto &ignorePath : ignorePaths) {
                        if (outInfo.path().indexOf(ignorePath) != -1) isNotIgnore = false;
                    }
                }

                if (isNotIgnore) {
                    QDir().mkpath(outInfo.path());
                    if (zipFile.open(QIODevice::ReadOnly)) {
                        if (outFile.open(QIODevice::WriteOnly)) {
                            outFile.write(zipFile.readAll());
                            outFile.close();
                        }
                        zipFile.close();
                    }
                }
            }
        }
        currentFile++;
        int progress = (currentFile * 99) / totalFiles;
        ui->installProgress->setValue(progress);
        QCoreApplication::processEvents();
    }

    zip.close();
    ui->installProgress->setValue(99);

    if (!isNewInstall) {
        for (auto &service : m_services) {
            if (!serviceController->startService(service.name, service.id)) {
                QMessageBox::warning(this, "服务启动失败", QString("启动%1服务失败, 请尝试以管理员身份运行或手动启动该服务").arg(service.nickname));
            }
        }
    }

    ui->installProgress->setValue(100);
    addLinkToDesktop(installPath.filePath("bin/configtool.exe"), "学生反馈系统配置工具");
    QMessageBox::information(this, "完成", "即将启动配置工具以完成最后的安装");
    QProcess::startDetached(installPath.filePath("bin/configtool.exe"));
    this->close();
}

void installer::startInstallWithStopServices()
{
    for (auto &service : m_services) {
        bool success = serviceController->stopService(service.name, service.id);
        if (!success) {
            if (serviceController->checkServiceNoSignal(service.name) != ServiceController::Stopped) {
                QMessageBox::warning(this, "服务停止失败", QString("停止%1服务失败, 请尝试以管理员身份运行或手动停止该服务").arg(service.nickname));
                return;
            }
        }
        m_services[service.id].isClosed = true;
    }
    startInstall();
}

void installer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
        event->accept();
    }
}

void installer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        this->move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void installer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

void installer::initDefaultInstallPath()
{
    QDir dDrive("D:/");
    if (dDrive.exists()) {
        installPath = dDrive.filePath("stufbsys");
        ui->installPathEditer->setText(installPath.absolutePath());
    } else {
        installPath = QDir("C:/").filePath("stufbsys");
        ui->installPathEditer->setText(installPath.absolutePath());
    }
}

void installer::addLinkToDesktop(const QString& filename, const QString& name) {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString linkPath = desktopPath + "/" + name + ".lnk";
    QFile::link(filename, linkPath);
}

bool installer::clearDir(QString path)
{
    if (path.isEmpty())
    {
        return false;
    }
    QDir dir(path);
    if (!dir.exists())
    {
        return false;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach(QFileInfo file, fileList)
    {
        if (file.isFile())
        {
            file.dir().remove(file.fileName());
        }
        else
        {
            clearDir(file.absoluteFilePath());
            file.dir().rmdir(file.absoluteFilePath());
        }
    }
    return true;
}
