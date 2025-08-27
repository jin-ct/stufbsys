#include "sslcertautoreq.h"
#include <QRandomGenerator>
#include <QProcess>
#include <QSettings>
#include <QDebug>

void SslCertAutoReq::run()
{
    switch (operType)
    {
    case Request:
        requestNewCert();
        break;
    case Check:
        checkCertExpirationTime();
        break;
    }
}

void SslCertAutoReq::requestNewCert()
{
    QStringList arguments = QStringList() 
        << "--source" << "manual"
        << "--host" << domainNames.join(',')
        << "--validation" << "filesystem"
        << "--webroot" << QDir::toNativeSeparators(installPath->filePath("web"))
        << "--emailaddress" << email
        << "--pemfilespath" << QDir::toNativeSeparators(installPath->filePath("vendors/nginx/cert"))
        << "--pemfilesname" << "cert"
        << "--script" << "reload-nginx.ps1"
        << "--store" << "pemfiles"
        << "--accepttos";
    bool success = executeCommand(installPath->filePath("vendors/win-acme/wacs.exe"), arguments);
    emit operFinished(success);
}

void SslCertAutoReq::checkCertExpirationTime()
{
    QString output;
    QProcess process;
    process.start(installPath->filePath("vendors/win-acme/wacs.exe"), QStringList() << "--list");
    if (process.waitForFinished(30000)) {
        output = QString::fromLocal8Bit(process.readAllStandardOutput());
        QDate expirationDate = parseCheckRes(output);
        emit checkExpTimeFinished(expirationDate);
        return;
    }
    emit checkExpTimeFinished(QDate(), false);
}

QDate SslCertAutoReq::parseCheckRes(const QString &output)
{
    QDate date;
    QString findStr = "due ";
    int idx = output.indexOf(findStr);
    if (idx != -1) {
        QString dateString = output.mid(idx + findStr.length(), 10);
        date = QDate::fromString(dateString, "yyyy/MM/dd");
    }
    return date;
}

SslCertAutoReq::SslCertAutoReq(QDir *installPathPtr, operationType operType, QStringList domainNames, QString email)
{
    this->installPath = installPathPtr;
    this->operType = operType;
    this->domainNames = domainNames;
    this->email = email;
}

bool SslCertAutoReq::executeCommand(const QString &command, const QStringList &arguments) {
    QProcess process;
    process.start(command, arguments);
    if (!process.waitForFinished(30000)) {
        qDebug() << command << "Execute Command error: " << process.errorString();
        return false;
    }
    return (process.exitCode() == 0);
}
