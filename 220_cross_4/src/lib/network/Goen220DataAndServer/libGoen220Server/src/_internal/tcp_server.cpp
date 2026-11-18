#include "tcp_server.h"
#include <QDebug>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(TCP_, "Goen220TcpServer")

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

    m_keepAliveTimer.setInterval(200);
    connect(&m_keepAliveTimer, &QTimer::timeout, this, &TcpServer::onKeepAliveTimerTimeout);
    setMaxPendingConnections(1);
}

TcpServer::~TcpServer()
{
    auto clients = m_clients.keys();

    for (auto c : std::as_const(clients)) {
        if (c->state() == QAbstractSocket::ConnectedState) {
            c->disconnectFromHost();
            c->waitForDisconnected();
        }
        delete c;
    }
}

void TcpServer::start(quint16 port)
{
    if (this->listen(QHostAddress::Any, port)) {
        m_keepAliveTimer.start();
        qCInfo(TCP_) << "Server started, listening on port" << port;
    } else {
        m_lastErr = this->serverError();
        m_lastErrSrc = ErrorSrc::Server;
        m_lastErrStr = this->errorString();
        qCCritical(TCP_) << "Could not start server:" << m_lastErrStr;
        emit errorOccured();
    }
}

void TcpServer::stop()
{
    m_keepAliveTimer.stop();

    close();

    auto clients = m_clients.keys();

    for(auto c : std::as_const(clients)){
        c->disconnectFromHost();
        c->deleteLater();
    }

    m_clients.clear();

    qCInfo(TCP_) << "Server stopped";
}

const QString &TcpServer::lastErrorString()
{
    return m_lastErrStr;
}

QAbstractSocket::SocketError TcpServer::lastError() const
{
    return m_lastErr;
}

TcpServer::ErrorSrc TcpServer::lastErrorSrc() const
{
    return m_lastErrSrc;
}

void TcpServer::setKeepAliveProcEnabled(bool state)
{
    m_keepAliveProcEnabled = state;

    if (m_keepAliveProcEnabled  && isListening()) {
        for(auto &cd : m_clients)
            cd.setAlive();
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    client->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    connect(client, &QTcpSocket::readyRead, this, &TcpServer::onClientReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
    connect(client, &QTcpSocket::errorOccurred, this, &TcpServer::onClientErrorOccurred);

    m_clients.insert(client, {});
    emit newClient(client);
}

void TcpServer::onClientReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client != nullptr) {
        QByteArray data = client->readAll();
        auto h = (const VdpHeader*) data.constData();

        if (h->cmd == 0x01 && h->status == 0 && h->byteCount == 0) { //keep alive message
            if (m_keepAliveProcEnabled) {
                m_clients[client].setAlive();
                return;
            }
        }

        emit dataReceived(client, data);
    }
}

void TcpServer::onClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        m_clients.remove(client);
        emit clientDisconnected(client);
        client->deleteLater();
    }
}

void TcpServer::onClientErrorOccurred(QAbstractSocket::SocketError error)
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client != nullptr) {
        if (error == QAbstractSocket::RemoteHostClosedError) {
            qCInfo(TCP_) << "Disconnected by client: " << client->peerAddress().toString() << ":" << client->errorString();
        } else {
            m_lastErr = error;
            m_lastErrSrc = ErrorSrc::Client;
            m_lastErrStr = client->errorString();
            qCCritical(TCP_) << "Error from: " << client->peerAddress().toString() << ":" << client->errorString();
            emit errorOccured();
        }
    }
}

void TcpServer::sendData(QTcpSocket *socket, const QByteArray &data)
{
    if (socket != nullptr && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(data);
        socket->flush();
    }
}

void TcpServer::disconnectClient(QTcpSocket *socket)
{
    if (socket != nullptr)
        socket->disconnectFromHost();
}

void TcpServer::onKeepAliveTimerTimeout()
{
    if (!m_keepAliveProcEnabled)
        return;

    QList<QTcpSocket*> deadClients;

    for (auto it = m_clients.cbegin(); it != m_clients.cend(); ++it) {
        if (!it.value().isAlive()) {
            qCWarning(TCP_) << "Keep alive timeout. Client:" << it.key()->peerAddress().toString();
            deadClients.append(it.key());
        }
    }

    for(auto c : deadClients)
        c->disconnectFromHost();
}
