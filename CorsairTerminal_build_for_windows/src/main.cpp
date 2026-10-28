#include "mainwindow.h"
#include <QApplication>

#include <QFile>

#include <QStyleFactory>

int main(int argc, char *argv[])
{
    // QApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    // QApplication::setFont(QFont("Arial", 10));

    QApplication a(argc, argv);

    QFile styleF;
   // styleF.setFileName(":/QSS_master/AMOLED.qss"); // V
   // styleF.setFileName(":/QSS_master/Aqua.qss");
   // styleF.setFileName(":/QSS_master/ConsoleStyle.qss"); // V
    // styleF.setFileName(":/QSS_master/ElegantDark.qss"); // V
   // styleF.setFileName(":/QSS_master/MacOS.qss");
    styleF.setFileName(":/QSS_master/ManjaroMix.qss"); // V
   // styleF.setFileName(":/QSS_master/MaterialDark.qss");
   // styleF.setFileName(":/QSS_master/NeonButtons.qss");
   // styleF.setFileName(":/QSS_master/Ubuntu.qss");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();

    qApp->setStyleSheet(qssStr);

    MainWindow w;
    w.show();

    return a.exec();
}
