#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QUdpSocket>

#include <libgoen220data.h>
#include <libgoen220_server.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void pushMessage(const QString& msg);

protected slots:

    void startServer(bool state);


private:
    Ui::MainWindow *ui;
    QSystemTrayIcon m_tray;


    QThread *m_serverThread = nullptr;
    Goen220::Server::Intrf *m_server = nullptr;
    Goen220::TlmDataModel *m_tlmModel = nullptr;


    bool m_quitEnabled = false;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};
