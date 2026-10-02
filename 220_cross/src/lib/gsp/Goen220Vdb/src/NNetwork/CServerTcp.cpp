#include "CServerTcp.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QCoreApplication>


Q_LOGGING_CATEGORY( serviceTcpServerState, "service.tcpServer.state")
Q_LOGGING_CATEGORY( serviceTcpServerTx, "service.tcpServer.tx")
Q_LOGGING_CATEGORY( serviceTcpServerRx, "service.tcpServer.rx")


using namespace NNetwork;


CServerTcp::CServerTcp( const unsigned short port, const unsigned char nodeId,
                                            QObject *parent):
    NCore::CNewThread( parent)
  , m_port( port)
  , m_nodeId( nodeId)
  , m_pTcpServer( nullptr)
  , m_pTcpSocket( nullptr)
{

}


CServerTcp::~CServerTcp()
{
    if( m_pTcpSocket)
    {
//        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        do
        {
            QCoreApplication::processEvents();
        }
        while( m_pTcpSocket);
    }

    if( m_pTcpServer)
    {
        m_pTcpServer->close();
        m_pTcpServer->deleteLater();
        m_pTcpServer = nullptr;
    }
}


void CServerTcp::sendData( const QByteArray data)
{
    if( !m_pTcpSocket)
        return;

//    qDebug() << "C<-S" << data.size() << data.toHex( ' ');
    qCDebug(serviceTcpServerTx) << m_nodeId << data.size() << data.toHex( ' ');
    m_pTcpSocket->write( data);
}


void CServerTcp::initializationInNewThread()
{
//    qDebug() << Q_FUNC_INFO;

    m_pTcpServer = new QTcpServer();
    connect( m_pTcpServer, &QTcpServer::newConnection, this, &CServerTcp::newConnectionIsAvailable);
    connect( m_pTcpServer, &QTcpServer::acceptError, this, &CServerTcp::acceptError);

    if( m_pTcpServer->listen( QHostAddress::Any, m_port) == false)
    {
//        qCritical() << Q_FUNC_INFO << "Error:   Listen TCP port. Port: " << m_port;
        qCritical(serviceTcpServerState) << tr( "Failed to open port. Port: %1").arg( m_port);
        emit serverError( QString::fromUtf8( "Error:   Listen TCP port. Port: %1").arg( m_port));
        return;
    }

    qCInfo(serviceTcpServerState) << tr( "TCP port open. Port: %1").arg( m_port);
}


void CServerTcp::newConnectionIsAvailable()
{
    QTcpSocket *tcpSocket = m_pTcpServer->nextPendingConnection();
    if( !tcpSocket)
        return;

    if( m_pTcpSocket)
    {
        emit stateClientChanged( m_nodeId, false);

//        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
//        m_pTcpSocket->deleteLater();
//        m_pTcpSocket = nullptr;
        do
        {
            QCoreApplication::processEvents();
        }
        while( m_pTcpSocket);
    }

    connect( tcpSocket, &QTcpSocket::readyRead, this, &CServerTcp::socketReadyRead);
    connect( tcpSocket, &QTcpSocket::disconnected, this, &CServerTcp::socketDisconnected);

    m_pTcpSocket = tcpSocket;
    qCInfo(serviceTcpServerState) << tr( "Client application connected. ID: %1").arg( m_nodeId);
    emit stateClientChanged( m_nodeId, true);
}


void CServerTcp::acceptError( QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    if( !m_pTcpServer)
        return;

    emit serverError( m_pTcpServer->errorString());
}


void CServerTcp::socketReadyRead()
{
    if( !m_pTcpSocket)
        return;

    while( true)
    {
        QByteArray data = m_pTcpSocket->readAll();
        if( data.size() < 1)
            return;

    //    qDebug() << "C->S" << data.size() << data.toHex( ' ');
        qCDebug(serviceTcpServerRx) << m_nodeId << data.size() << data.toHex( ' ');
        emit receivedData( data);
    }
}


void CServerTcp::socketDisconnected()
{
    qCInfo(serviceTcpServerState) << tr( "Client application disconnected. ID: %1").arg( m_nodeId);
    emit stateClientChanged( m_nodeId, false);

    if( !m_pTcpSocket)
        return;

    m_pTcpSocket->deleteLater();
    m_pTcpSocket = nullptr;
}
