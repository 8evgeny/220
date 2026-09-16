#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QTimer>
#include "main_app.h"
#include <gst/gst.h>

void handleSegfaultMain(int sig) {
    qDebug() << "Segfault occurred. Signal: " << sig;
    // Print stack trace if possible
    exit(1);
}

#include <QDebug>
#include <QDateTime>
#include <QMessageLogContext>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    switch (type) {
    case QtDebugMsg:
       // fprintf(stderr, "%s [DEBUG] %s (%s:%u, %s)\n", timestamp.toLocal8Bit().constData(), localMsg.constData(),
       //        context.file, context.line, context.function);
        fprintf(stderr, "%s [DEBUG] %s (%s)\n", timestamp.toLocal8Bit().constData(), localMsg.constData(),
                context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "%s [INFO] %s\n", timestamp.toLocal8Bit().constData(), localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s [WARNING] %s\n", timestamp.toLocal8Bit().constData(), localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s [CRITICAL] %s\n", timestamp.toLocal8Bit().constData(), localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s [FATAL] %s\n", timestamp.toLocal8Bit().constData(), localMsg.constData());
        abort();
    }
}

static Main_app *app_inst;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // return a.exec();

    gst_init(&argc, &argv);
    qInstallMessageHandler(customMessageHandler);
    Main_app app(&a);

    app_inst = &app;

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.
   // QTimer::singleShot(5000, &a, &QCoreApplication::quit);

    signal(SIGSEGV, handleSegfaultMain);
    return a.exec();


   // signal(SIGINT, handleSignal);
   // signal(SIGTERM, handleSignal);
}

