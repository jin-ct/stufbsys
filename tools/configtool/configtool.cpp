#include "configtool.h"
#include "./ui_configtool.h"
#include <QDebug>
#include <QMovie>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QDesktopServices>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QInputDialog>
#include "databasesetpassword.h"
#include "sslcertautoreq.h"

ConfigTool::ConfigTool(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConfigTool)
{
    ui->setupUi(this);

    // 加载动画
    QMovie *loadingMov = new QMovie(":/img/loading.gif");
    loadingMov->setScaledSize(QSize(240,240));
    ui->loadingImg->setMovie(loadingMov);
    loadingMov->start();

    // 加载配置文件
    QSettings settings("conf/config.ini", QSettings::IniFormat);
    postgresPassword = settings.value("postgres_password").toString();
    domainNames = settings.value("domain_names").toStringList();
    ui->domainNameEditer->setText(domainNames.join("\n"));
    isOpenSsl = settings.value("open_ssl").toBool();
    isUseAutoCert = settings.value("use_auto_cert").toBool();
    if (!postgresPassword.isEmpty()) ui->resetPgsqlPasswordBtn->hide();

    // 初始化服务信息
    installPath = new QDir(QCoreApplication::applicationDirPath());
    installPath->cdUp();
    m_services = {
        {
            0,
            "StuFBSysPostgreSQL",
            QString("%1/vendors/pgsql/bin").arg(installPath->absolutePath()),
            ServiceController::Stopped
        },
        {
            1,
            "StuFBSysNginx",
            installPath->filePath("vendors/nginx/nginx"),
            ServiceController::Stopped
        },
        {
            2,
            "StuFBSysBackend",
            installPath->filePath("server/stufbsys"),
            ServiceController::Stopped
        }
    };

    // 服务管理
    serviceController = new ServiceController();
    serviceControllerThread = new QThread(this);
    serviceController->moveToThread(serviceControllerThread);
    connect(serviceController, &ServiceController::serviceStatusChanged, this, &ConfigTool::updateServiceStatus);
    connect(serviceController, &ServiceController::finished, this, &ConfigTool::onOperationFinished);
    connect(this, &ConfigTool::checkService, serviceController, &ServiceController::checkService);
    connect(this, &ConfigTool::startService, serviceController, &ServiceController::startService);
    connect(this, &ConfigTool::stopService, serviceController, &ServiceController::stopService);
    connect(this, &ConfigTool::restartService, serviceController, &ServiceController::restartService);
    connect(this, &ConfigTool::createService, serviceController, &ServiceController::createService);
    connect(this, &ConfigTool::createPgSqlService, serviceController, &ServiceController::createPgSqlService);
    serviceControllerThread->start();

    // 更新状态
    for (auto &service : m_services) {
        emit checkService(service.name, service.id);
    }

    // 按钮操作
    connect(ui->startServerBtn, &QPushButton::clicked, this, [=]() {
        emit startService(m_services[2].name, m_services[2].id);
        ui->startServerBtn->setEnabled(false);
    });
    connect(ui->stopServerBtn, &QPushButton::clicked, this, [=]() {
        emit stopService(m_services[2].name, m_services[2].id);
        ui->stopServerBtn->setEnabled(false);
    });
    connect(ui->restartServerBtn, &QPushButton::clicked, this, [=]() {
        emit restartService(m_services[2].name, m_services[2].id);
        ui->restartServerBtn->setEnabled(false);
        ui->stopServerBtn->setEnabled(false);
    });
    connect(ui->startNginxServerBtn, &QPushButton::clicked, this, [=]() {
        emit startService(m_services[1].name, m_services[1].id);
        ui->startNginxServerBtn->setEnabled(false);
    });
    connect(ui->stopNginxServerBtn, &QPushButton::clicked, this, [=]() {
        emit stopService(m_services[1].name, m_services[1].id);
        ui->stopNginxServerBtn->setEnabled(false);
    });
    connect(ui->restartNginxServerBtn, &QPushButton::clicked, this, [=]() {
        emit restartService(m_services[1].name, m_services[1].id);
        ui->restartNginxServerBtn->setEnabled(false);
        ui->stopNginxServerBtn->setEnabled(false);
    });
    connect(ui->startSqlServerBtn, &QPushButton::clicked, this, [=]() {
        emit startService(m_services[0].name, m_services[0].id);
        ui->startSqlServerBtn->setEnabled(false);
    });
    connect(ui->stopSqlServerBtn, &QPushButton::clicked, this, [=]() {
        emit stopService(m_services[0].name, m_services[0].id);
        ui->stopSqlServerBtn->setEnabled(false);
    });
    connect(ui->restartSqlServerBtn, &QPushButton::clicked, this, [=]() {
        emit restartService(m_services[0].name, m_services[0].id);
        ui->restartSqlServerBtn->setEnabled(false);
        ui->stopSqlServerBtn->setEnabled(false);
    });
    // 打开日志目录
    connect(ui->openServerLogDirBtn, &QPushButton::clicked, this, [=](){
        QDesktopServices::openUrl(QUrl(installPath->absolutePath() + "/server/log", QUrl::TolerantMode));
    });
    connect(ui->openNginxServerLogDir, &QPushButton::clicked, this, [=](){
        QDesktopServices::openUrl(QUrl(installPath->absolutePath() + "/vendors/nginx/logs", QUrl::TolerantMode));
    });
    connect(ui->openSqlServerLogDirBtn, &QPushButton::clicked, this, [=](){
        QDesktopServices::openUrl(QUrl(installPath->absolutePath() + "/db_data/log", QUrl::TolerantMode));
    });
    // 打开Nginx配置文件
    connect(ui->showNginxConfig, &QPushButton::clicked, this, [=](){
        QDesktopServices::openUrl(QUrl(installPath->absolutePath() + "/vendors/nginx/conf/nginx.conf", QUrl::TolerantMode));
    });
    // 查看postgres密码
    connect(ui->showSqlServerPasswored, &QPushButton::clicked, this, [=](){
        QMessageBox ms(this);
        ms.setTextInteractionFlags(Qt::TextSelectableByMouse);
        ms.setWindowTitle("数据库postgres账号密码查看");
        ms.setText(postgresPassword);
        ms.exec();
    });
    // 初始化postgres密码
    connect(ui->resetPgsqlPasswordBtn, &QPushButton::clicked, this, [=](){
        if (!postgresPassword.isEmpty()) return;
        QMessageBox::StandardButton btn = QMessageBox::question(this, "重置postgres密码", "确定要重置PgSql数据库postgres账号的密码吗？（密码将随机生成）");
        if (btn == QMessageBox::Yes) {
            resetPgSqlPassword(0);
        }
    });
    // 保存域名
    connect(ui->saveDomainNameBtn, &QPushButton::clicked, this, [=](){
        ui->saveDomainNameBtn->setEnabled(false);
        domainNames = ui->domainNameEditer->toPlainText().split("\n", Qt::SkipEmptyParts);
        QSettings settings(QString("%1/bin/conf/config.ini").arg(installPath->absolutePath()), QSettings::IniFormat);
        settings.setValue("domain_names", domainNames);
        settings.sync();
        if(editNginxDomainConfig()) {
            QMessageBox::information(this, "保存成功", "域名保存成功！");
            ui->saveDomainNameBtn->setEnabled(true);
        }
    });
    // 设置自定义证书
    connect(ui->setCerBtn, &QPushButton::clicked, this, [=](){
        QMessageBox pemEditer(this);
        pemEditer.setWindowTitle("提示");
        pemEditer.setText("请直接在弹出的文件夹中编辑或替换证书(cert-chain.pem)和密钥(cert-key.pem)，请不要改变文件名");
        pemEditer.exec();
        QDesktopServices::openUrl(QUrl(installPath->absolutePath() + "/vendors/nginx/cert", QUrl::TolerantMode));
        ui->applyCustomCerBtn->show();
    });
    ui->applyCustomCerBtn->hide();
    connect(ui->applyCustomCerBtn, &QPushButton::clicked, this, [=](){
        if (openNginxSsl()) {
            ui->applyCustomCerBtn->hide();
            QMessageBox::information(this, "应用成功", "自定义证书应用成功");
        }
    });
    // 查看证书
    connect(ui->showCerBtn, &QPushButton::clicked, this, &ConfigTool::showCert);
    // 一键申请证书
    connect(ui->applyCerBtn, &QPushButton::clicked, this, &ConfigTool::applyCer);
    // 启动时检查证书状态
    if (isUseAutoCert) {
        SslCertAutoReq *sslCertAutoReq = new SslCertAutoReq(installPath, SslCertAutoReq::Check, domainNames);
        connect(sslCertAutoReq, &SslCertAutoReq::checkExpTimeFinished, this, [=](const QDate &expirationDate, bool success){
            if (success) {
                if (expirationDate.isValid()) {
                    int expirationDays = QDate::currentDate().daysTo(expirationDate);
                    ui->certState->setText(QString("%1天后").arg(expirationDays));
                } else {
                    ui->certState->setText("异常");
                }
            } else {
                ui->certState->setText("获取失败");
            }
            sslCertAutoReq->quit();
            sslCertAutoReq->deleteLater();
        });
        sslCertAutoReq->start();
    } else {
        ui->certState->setText("未申请");
    }
}

void ConfigTool::setStatusToUi(ServiceController::ServiceStatus status, int id)
{
    switch (id)
    {
    case 0:
        ui->sqlState->setCurrentIndex(status == ServiceController::Running ? 1 : 0);
        ui->startSqlServerBtn->setEnabled(status == ServiceController::Stopped);
        ui->stopSqlServerBtn->setEnabled(status == ServiceController::Running);
        ui->restartSqlServerBtn->setEnabled(status == ServiceController::Running);
        break;
    case 1:
        ui->nginxState->setCurrentIndex(status == ServiceController::Running ? 1 : 0);
        ui->startNginxServerBtn->setEnabled(status == ServiceController::Stopped);
        ui->stopNginxServerBtn->setEnabled(status == ServiceController::Running);
        ui->restartNginxServerBtn->setEnabled(status == ServiceController::Running);
        break;
    case 2:
        ui->serverState->setCurrentIndex(status == ServiceController::Running ? 1 : 0);
        ui->startServerBtn->setEnabled(status == ServiceController::Stopped);
        ui->stopServerBtn->setEnabled(status == ServiceController::Running);
        ui->restartServerBtn->setEnabled(status == ServiceController::Running);
        break;
    }
}

void ConfigTool::updateServiceStatus(const QString &serviceName, ServiceController::ServiceStatus status, int id)
{
    qDebug() << "Service status returned:" << serviceName;
    ServiceInfo &service = m_services[id];
    service.status = status;
    setStatusToUi(status, id);
    if (service.status == ServiceController::Error) {
        if (!service.isCheckStatusRetrying) {
            m_services[id].isCheckStatusRetrying = true;
            QTimer::singleShot(5000, this, [=]() { //5秒后重试
                emit checkService(service.name, service.id);
            });
            return;
        }
        QMessageBox::warning(this, "获取服务状态错误", "获取服务状态错误，请检查配置文件并尝试重新启动系统 ("+ m_services[id].name + ")");
        qWarning() << "Get service status error:" << m_services[id].name;
        service.isCheckStatusRetrying = false;
    } else if (service.status == ServiceController::uninstalled) {
        ui->loadingText->setText("正在创建服务");
        qDebug() << "Service uninstalled:" << service.name;
        if (service.id == 0) {
            emit createPgSqlService(service.name, service.binPath, QString("%1/db_data").arg(installPath->absolutePath()), id);
        } else {
            emit createService(service.name, service.binPath, id);
        }
        if (service.id == 1) openFirewallWebPort();  // 若为Nginx服务未创建则打开防火墙80和443端口
    }
    else{
        ui->pageNotLoading->setCurrentIndex(1);
    }
}

void ConfigTool::onOperationFinished(ServiceController::ServiceOperationType type, bool success, int id)
{
    switch (type)
    {
    case ServiceController::Create:
        if (success) {
            emit checkService(m_services[id].name, m_services[id].id);
        } else{
            QMessageBox::warning(this, "服务创建失败", "请尝试以管理员身份运行，或使用部署程序重新安装");
            qWarning() << "Service create error:" << m_services[id].name;
        }
        break;
    case ServiceController::PgSqlCreate:
        if (success) {
            emit checkService(m_services[id].name, m_services[id].id);
            resetPgSqlPassword(id);
        } else {
            QMessageBox::warning(this, "PgSql服务创建失败", "请尝试以管理员身份运行，或使用部署程序重新安装");
            qWarning() << "PgSqlService create error";
        }
        break;
    case ServiceController::Start:
        if (success) {
            emit checkService(m_services[id].name, m_services[id].id);
        } else {
            QMessageBox::warning(this, "服务启动失败", "服务启动失败，请尝试以管理员身份运行");
            qWarning() << "Service start error:" << m_services[id].name;
        }
        break;
    case ServiceController::Stop:
        if (success) {
            emit checkService(m_services[id].name, m_services[id].id);
        } else {
            QMessageBox::warning(this, "服务停止失败", "服务停止失败，请尝试以管理员身份运行");
            qWarning() << "Service stop error:" << m_services[id].name;
        }
        break;
    case ServiceController::Restart:
        if (success) {
            emit checkService(m_services[id].name, m_services[id].id);
        } else {
            QMessageBox::warning(this, "服务重启失败", "服务重启失败，请尝试以管理员身份运行");
            qWarning() << "Service restart error:" << m_services[id].name;
        }
        break;
    }
}

void ConfigTool::showCert()
{
    QFile pemFile(installPath->filePath("vendors/nginx/cert/cert-chain.pem"));
    QFile keyFile(installPath->filePath("vendors/nginx/cert/cert-key.pem"));
    if (pemFile.open(QIODevice::ReadOnly | QIODevice::Text) && keyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString pemContent = pemFile.readAll();
        QString keyContent = keyFile.readAll();
        pemFile.close();
        keyFile.close();

        QDialog *showCert = new QDialog(this);
        QTextEdit *cerEdit = new QTextEdit(showCert);
        cerEdit->setReadOnly(true);
        cerEdit->setText(pemContent);
        cerEdit->setMinimumSize(600, 400);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(cerEdit);
        showCert->setLayout(layout);

        showCert->setWindowTitle("证书(PEM格式) 关闭该窗口后显示密钥");
        cerEdit->setText(pemContent);
        showCert->exec();
        showCert->setWindowTitle("密钥(KEY)");
        cerEdit->setText(keyContent);
        showCert->exec();
    }
}

void ConfigTool::applyCer()
{
    QString email = QInputDialog::getText(this, "请输入邮箱", "需要邮箱用于创建Let's Encrypt账号：", QLineEdit::Normal);
    if (email.isEmpty()) {
        QMessageBox::warning(this, "邮箱未填写", "请填写邮箱后再申请证书。");
        return;
    }
    ui->applyCerBtn->setEnabled(false);
    ui->applyCerBtn->setText("申请中...");
    SslCertAutoReq *sslCertAutoReq = new SslCertAutoReq(installPath, SslCertAutoReq::Request, domainNames, email);
    connect(sslCertAutoReq, &SslCertAutoReq::operFinished, this, [=](bool success){
        if (success) {
            openNginxSsl();
            isUseAutoCert = true;
            QSettings settings(QString("%1/bin/conf/config.ini").arg(installPath->absolutePath()), QSettings::IniFormat);
            settings.setValue("use_auto_cert", isUseAutoCert);
            settings.sync();
            QMessageBox::information(this, "申请成功", "证书申请成功！");
        } else {
            QMessageBox::warning(this, "申请失败", "证书申请失败，请重试！");
            QMessageBox::StandardButton btn = QMessageBox::question(this, "查看申请日志", "是否查看申请日志？");
            if (btn == QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl("C:/ProgramData/win-acme/acme-v02.api.letsencrypt.org/Log", QUrl::TolerantMode));
            }
        }
        ui->applyCerBtn->setEnabled(true);
        ui->applyCerBtn->setText("一键申请Let’s Encrypt证书");
        sslCertAutoReq->wait();
        sslCertAutoReq->quit();
        sslCertAutoReq->deleteLater();
    });
    sslCertAutoReq->start();
}

ConfigTool::~ConfigTool()
{
    delete ui;
    serviceControllerThread->quit();
}

void ConfigTool::resetPgSqlPassword(int id)
{
    ui->resetPgsqlPasswordBtn->setEnabled(false);
    ui->resetPgsqlPasswordBtn->setText("申请中");
    DatabaseSetPassword *dbSP = new DatabaseSetPassword(installPath, serviceController, m_services[id].name, id);
    connect(dbSP, &DatabaseSetPassword::operFinished, this, [=](bool success, QString newPassword) {
        if (!success) {
            QMessageBox::warning(this, "PgSql密码初始化失败", "数据库密码初始化失败，请尝试手动初始化密码");
            qWarning() << "Pgsql password reset error";
        } else {
            this->postgresPassword = newPassword;
            ui->resetPgsqlPasswordBtn->hide();
        }
        dbSP->wait();
        dbSP->quit();
        dbSP->deleteLater();
        updateServerConfig(newPassword);
    });
    dbSP->start();
}

void ConfigTool::openFirewallWebPort()
{
    QString command = QString("netsh advfirewall firewall add rule name=\"Http Web\" dir=in action=allow protocol=TCP localport=80");
    QString command_https = QString("netsh advfirewall firewall add rule name=\"Https Web\" dir=in action=allow protocol=TCP localport=443");
    bool success = executeCommand("cmd.exe", QStringList() << "/C" << command)&&
                   executeCommand("cmd.exe", QStringList() << "/C" << command_https);
    if (!success) {
        QMessageBox::warning(this, "防火墙规则添加失败", "防火墙规则添加失败，请尝试手动开放80和443端口");
        qWarning() << "firewall rule add error";
    }
}

bool ConfigTool::updateNginxConfig(const QString &filePath, std::function<void(QString &content)> editConfig)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString content = file.readAll();
        file.seek(0);
        editConfig(content);
        file.resize(0);
        file.write(content.toUtf8());
        file.close();
        return true;
    } else {
        QMessageBox::warning(this, "配置文件打开失败", "Nginx配置文件打开失败，请检查文件权限");
        qWarning() << "Nginx config file open error, filePath: "<< filePath;
    }
    return false;
}

bool ConfigTool::editNginxDomainConfig()
{
    bool success = updateNginxConfig(installPath->filePath("vendors/nginx/conf/nginx.conf"), [=](QString &content) {
        QStringList lines = content.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed().startsWith("server_name")) {
                int idx = lines[i].indexOf("server_name");
                QString newLine = lines[i].left(idx + QString("server_name").length());
                newLine += "  " + domainNames.join(" ") + ";";
                lines[i] = newLine;
            }
        }
        content = lines.join('\n');
    });
    if (success) emit restartService(m_services[1].name, 1); // 重启Nginx服务以应用配置
    return success;
}

bool ConfigTool::openNginxSsl()
{
    if (isOpenSsl) {
        emit restartService(m_services[1].name, 1);  // 重启Nginx服务以应用证书
        return true;
    }
    QFile templateFile(installPath->filePath("vendors/nginx/conf/ssl/ssl_server_template.conf"));
    if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QString templateContent = templateFile.readAll();
    bool success = updateNginxConfig(installPath->filePath("vendors/nginx/conf/nginx.conf"), [=](QString &content) {
        QStringList lines = content.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed().startsWith("server_name")) {
                lines.insert(i + 1, "\n        rewrite ^(.*)$ https://$host$1 permanent;"); // 强制跳转https
                break;
            }
        }
        for (int i = lines.size() - 1; i >= 0; --i) {
            if (lines[i].trimmed().startsWith("}")) {
                lines.insert(i, "\n\n" + templateContent);
                break;
            }
        }
        content = lines.join('\n');
    });
    if (success){ //保存ssl状态
        isOpenSsl = true;
        QSettings settings(QString("%1/bin/conf/config.ini").arg(installPath->absolutePath()), QSettings::IniFormat);
        settings.setValue("open_ssl", isOpenSsl);
        settings.sync();
        success = (editNginxDomainConfig() && success);
    }
    return success;
}

void ConfigTool::updateServerConfig(QString newDbPassword)
{
    QString configPath = installPath->filePath("server/config.json");
    QFile configFile(configPath);
    if (configFile.open(QIODevice::ReadWrite)) {
        QByteArray data = configFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject config = doc.object();

        QJsonArray db = config["db_clients"].toArray();
        QJsonObject db_0 = db[0].toObject();
        db_0["passwd"] = newDbPassword;
        db[0] = db_0;
        config["db_clients"] = db;

        // 设置随机生成的jwt密钥
        QString jwt_secret = getRandomString(32);
        QJsonArray jwt = config["plugins"].toArray();
        QJsonObject jwt_2 = jwt[2].toObject();
        QJsonObject jwt_2_c = jwt_2["config"].toObject();
        jwt_2_c["secret"] = jwt_secret;
        jwt_2["config"] = jwt_2_c;
        jwt[2] = jwt_2;
        config["plugins"] = jwt;
        
        // 设置随机生成的encryptText密钥 (在后端encryptText() 中使用)
        QString encryptTextKey = getRandomString(32);
        QString encryptTextIv = getRandomString(16);
        QJsonObject custom_conf = config["custom_config"].toObject();
        custom_conf["encryptTextKey"] = encryptTextKey;
        custom_conf["encryptTextIv"] = encryptTextIv;
        config["custom_config"] = custom_conf;

        configFile.seek(0);
        configFile.resize(0);
        configFile.write(QJsonDocument(config).toJson());
        configFile.close();

        emit restartService(m_services[2].name, 2);
    } else {
        qWarning() << "Failed to open server's config.json";
        QMessageBox::warning(this, "打开配置文件失败", "打开服务端配置文件失败，请检查文件是否存在及其权限");
    }
}

bool ConfigTool::executeCommand(const QString &command, const QStringList &arguments) {
    QProcess process;
    process.start(command, arguments);
    if (!process.waitForFinished(30000)) {
        qDebug() << command << "Execute Command error: " << process.errorString();
        return false;
    }
    return true;
}

QString ConfigTool::getRandomString(const int n)
{
    QString str;
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < n; ++i) {
        int index = QRandomGenerator::global()->bounded(sizeof(alphanum) - 1);
        str.append(alphanum[index]);
    }
    return str;
}
