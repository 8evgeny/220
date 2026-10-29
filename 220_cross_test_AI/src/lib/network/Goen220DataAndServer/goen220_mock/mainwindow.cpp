#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCloseEvent>
#include <QThread>
#include <QTimer>

static MainWindow *s_mw = nullptr;

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    s_mw->pushMessage(msg);
} /* myMessageHandler */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, &QAction::triggered, this, [this]
            {
                m_quitEnabled = true;
                qApp->quit();
            } /* lambda */);

    connect(ui->actionShowHide, &QAction::triggered, this,
            [this]
            {
                setVisible(!isVisible());
            } /* lambda */);

    QIcon icn("://assets/logo.png");

    setWindowIcon(icn);

    m_tray.setIcon(windowIcon());
    m_tray.setVisible(true);
    m_tray.setToolTip("ГОЭН 220. Имимитатор сервера. Dbl-Click для отображения");

    m_tray.setContextMenu(ui->menuAppCtrl);

    connect(&m_tray, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason rsn)
            {
                switch (rsn)
                {
                case QSystemTrayIcon::Unknown:
                    break;
                case QSystemTrayIcon::Context:
                    break;
                case QSystemTrayIcon::DoubleClick:
                    setVisible(!isVisible());
                    break;
                case QSystemTrayIcon::Trigger:
                    break;
                case QSystemTrayIcon::MiddleClick:
                    break;
                } /* switch */
            } /* lambda */);

    // qDebug() << " ---- sz TM:" << sizeof(Goen220::Tlm);

    QThread::currentThread()->setObjectName("***MAIN THREAD***");

    s_mw = this;

    qInstallMessageHandler(myMessageHandler);

    m_tlmModel = Goen220::TlmDataModel::createEditableModel(this);
    ui->tvTlm->setModel(m_tlmModel);

    // m_server = Goen220::Server::create(this);

    m_server = Goen220::Server::createOnThread();
    m_serverThread = m_server->thread();

    if (m_serverThread == QThread::currentThread())
    {
        qFatal() << "WARNING. SERVER THREAD:" << m_serverThread->objectName();
        return;
    } /* if */

    m_server->setTlmSrc(&m_tlmModel->rawData());

    connect(ui->pbSuicide, &QPushButton::clicked, this, []
            {
                quint8 *p = nullptr;
                *p = 42;
            } /* lambda */);

    connect(ui->pbStart, &QPushButton::clicked, this, &MainWindow::startServer);

    ui->chbKeepAliveProc->setChecked(m_server->isKeepAliveProcEnabled());

    connect(ui->chbKeepAliveProc, &QCheckBox::checkStateChanged, m_server,
            &Goen220::Server::Intrf::setKeepAliveProcEnabled);

    connect(
        m_server, &Goen220::Server::Intrf::errorOccured, this,
        [this]
        {
            auto e = m_server->lastError();
            int ec = m_server->lastErrorCode();
            auto str = m_server->lastErrorString();
            qCritical() << "ERROR. " << e << "code:" << ec << str;

            if (e == Goen220::Server::Intrf::Error::TcpServer)
                startServer(false);
        } /* lambda */,
        Qt::QueuedConnection);

    connect(
        m_server, &Goen220::Server::Intrf::clientConnectionStateChanged, this,
        [this](bool state)
        {
            qDebug() << "Client" << (state ? "connected." : "diconnected.") << "Peer"
                     << m_server->client().toString();
        } /* lambda */,
        Qt::QueuedConnection);

    connect(
        m_server, &Goen220::Server::Intrf::cmdReceived, this,
        [this](Goen220::Cmd cmd, QByteArray pld, bool ackRequired)
        {
            qDebug() << "Cmd:" << cmd << pld.toHex();
            if (ackRequired)
                m_server->cmdAck(cmd, Goen220::CmdAck::Ok);
        } /* lambda */,
        Qt::QueuedConnection);

    QTimer::singleShot(1000, this, [this]
                       {
                           startServer(true);
                       } /* lambda */);

    // QTimer::singleShot(2000, this, [this]{setVisible(false);});
} /* MainWindow::MainWindow */

MainWindow::~MainWindow()
{

    if (m_serverThread == QThread::currentThread())
    {
        delete m_server;
    } /* if */
    else
    {

        Goen220::Server::removeServer(m_server);

        m_serverThread->requestInterruption(); /// just for safety
        m_serverThread->quit();
        if (!m_serverThread->wait(4000))
        {
            m_serverThread->terminate();
            m_serverThread->wait(4000);
        } /* if */

        delete m_serverThread;
    } /* else */

    delete ui;
} /* MainWindow::~MainWindow */

void MainWindow::pushMessage(const QString &msg)
{
    ui->lwLog->addItem(msg);
    ui->lwLog->scrollToBottom();
} /* MainWindow::pushMessage */

void MainWindow::startServer(bool state)
{
    m_server->setListening(state);

    ui->pbStart->setText(state ? "SERVER ON" : "SERVER OFF");
    ui->pbStart->setChecked(state);
} /* MainWindow::startServer */

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_quitEnabled)
    {
        event->ignore();
        setVisible(false);
    } /* if */
    else
    {
        qInstallMessageHandler(nullptr);
        event->accept();
    } /* else */
} /* MainWindow::closeEvent */
