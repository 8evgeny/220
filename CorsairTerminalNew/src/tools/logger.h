#ifndef LOGGER_H
#define LOGGER_H

#include <QScopedPointer>
#include <QFile>
#include <QLoggingCategory>
#include <QDebug>
#include <QObject>

Q_DECLARE_LOGGING_CATEGORY(logExample)

extern void messageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg);

class Logger: public QObject
{
    Q_OBJECT
private:
    QScopedPointer<QFile> file;
public:
    Logger(const QString & filename = QString("log.txt"));
    ~Logger();
    void messageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg);
};

#endif // LOGGER_H
