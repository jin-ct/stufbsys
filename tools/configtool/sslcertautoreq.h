#ifndef SSLCERTAUTOREQ_H
#define SSLCERTAUTOREQ_H

#include <QThread>
#include <QDir>
#include <QDate>

class SslCertAutoReq : public QThread {
    Q_OBJECT
public:
    enum operationType { Request, Check };

    SslCertAutoReq(QDir *installPathPtr, operationType operType, QStringList domainNames = QStringList(), QString email = "");

signals:
    void operFinished(bool success);
    void checkExpTimeFinished(const QDate &expirationDate, bool success = true);

protected:
    void run() override;
    void requestNewCert();
    void checkCertExpirationTime();
    QDate parseCheckRes(const QString &output);
    bool executeCommand(const QString &command, const QStringList &arguments = QStringList());

    QDir *installPath;
    operationType operType;
    QStringList domainNames;
    QString email;
};

#endif // SSLCERTAUTOREQ_H
