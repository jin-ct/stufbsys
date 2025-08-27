#include "configtool.h"

#include <QApplication>

void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) 
{ 
    QFile file("log/configtool.log"); 
    file.open(QIODevice::WriteOnly | QIODevice::Append); 
    QTextStream out(&file); 
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "); 
    switch (type) { 
    case QtDebugMsg: 
        out << "Debug: "; 
        break; 
    case QtInfoMsg: 
        out << "Info: "; 
        break; 
    case QtWarningMsg: 
        out << "Warning: "; 
        break; 
    case QtCriticalMsg: 
        out << "Critical: "; 
        break; 
    case QtFatalMsg: 
        out << "Fatal: "; 
        break; 
    } 
    out << msg << " (" << context.file << ":" << context.line << ", " << context.function << ")\n"; 
    file.flush(); 
    file.close(); 
} 

int main(int argc, char *argv[])
{
    qInstallMessageHandler(logOutput);
    qDebug() << "ConfigTool opened";
    QApplication a(argc, argv);
    ConfigTool w;
    w.show();
    return a.exec();
}
