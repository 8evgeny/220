#include "logger.h"

#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QLoggingCategory>
#include <QMessageLogger>

Q_LOGGING_CATEGORY(logExample, "Example")

void messageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    static Logger logger("log.txt");
    logger.messageHandler(type, context, msg);
}

Logger::Logger(const QString & filename):
    QObject(nullptr)
{
    // file.reset(new QFile(filename));
    // file.data()->open(QFile::WriteOnly | QFile::Append | QFile::Text);
    // if(!file.data()->isOpen())
    // {
    //     qCritical() << "Can not open log file" << endl;
    //     return;
    // }
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
#ifdef DEBUG
    qInfo() << "=====================================================================================================================";
    qInfo() << "================================================" << datetime.toStdString().c_str() << "================================================";
    qInfo() << "=====================================================================================================================";
#endif
    // QTextStream out(file.data());
    // out << "===================================================================================================================" << endl;
    // out << "================================================" << datetime << "================================================" << endl;
    // out << "===================================================================================================================" << endl;
    // out.flush();
}
Logger::~Logger()
{
    // if(file.data()->isOpen())
    // {
    //     file.data()->flush();
    //     file.data()->close();
    // }
    // fflush(stderr);
    // fflush(stdout);
}
void Logger::messageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    QString datetime = QDateTime::currentDateTime().toString("<yyyy-MM-dd hh:mm:ss.zzz>");
#ifdef DEBUG
    switch (type)
    {
    case QtInfoMsg:     qInfo()     << datetime.toStdString().c_str() << "INF:" << context.category << ":" << msg.toStdString().c_str(); fflush(stderr); break;
    case QtDebugMsg:    qDebug()    << datetime.toStdString().c_str() << "DBG:" << context.category << ":" << msg.toStdString().c_str(); break;
    case QtWarningMsg:  qWarning()  << datetime.toStdString().c_str() << "WRN:" << context.category << ":" << msg.toStdString().c_str(); break;
    case QtCriticalMsg: qCritical() << datetime.toStdString().c_str() << "CRT:" << context.category << ":" << msg.toStdString().c_str(); break;
    case QtFatalMsg:    qFatal("%s FTL: %s: %s\n", datetime.toStdString().c_str(), context.category, msg.toStdString().c_str());
    }
//    fflush(stderr);
//    fflush(stdout);
#endif
//     if(file.data()->isOpen())
//     {
//         QTextStream out(file.data());
//         switch (type)
//         {
//         case QtInfoMsg:     out << datetime << " INF: " << context.category << " : " << msg << endl; break;
//         case QtDebugMsg:    out << datetime << " DBG: " << context.category << " : " << msg << endl; break;
//         case QtWarningMsg:  out << datetime << " WRN: " << context.category << " : " << msg << endl; break;
//         case QtCriticalMsg: out << datetime << " CRT: " << context.category << " : " << msg << endl; break;
//         case QtFatalMsg:    out << datetime << " FTL: " << context.category << " : " << msg << endl; break;
//         }
// //        out.flush();
//     }
}
