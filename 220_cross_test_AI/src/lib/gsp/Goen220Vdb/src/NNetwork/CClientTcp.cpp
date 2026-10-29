#include "CClientTcp.h"

#include <QTcpSocket>
#include <QDebug>

using namespace NNetwork;

CClientTcp::CClientTcp(QObject *parent)
    : NCore::CNewThread(parent), m_pTcpSocket(nullptr), m_hostName(QString::fromUtf8("localhost")),
      m_hostPort(19021), m_isConnected(false)
{

} /* CClientTcp::CClientTcp */

void CClientTcp::connectToServer(const bool con)
{
    if (!m_pTcpSocket)
        return;

    if (con)
        m_pTcpSocket->connectToHost(m_hostName, m_hostPort);
    else
        m_pTcpSocket->disconnectFromHost();
} /* CClientTcp::connectToServer */

void CClientTcp::sendData(const QByteArray data)
{
    if (!m_pTcpSocket || m_isConnected == false)
        return;

    m_pTcpSocket->write(data);
} /* CClientTcp::sendData */

void CClientTcp::initializationInNewThread()
{
    m_pTcpSocket = new QTcpSocket();

    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &CClientTcp::socketReadyRead);

    connect(m_pTcpSocket, &QTcpSocket::errorOccurred, this, &CClientTcp::acceptError);

    connect(m_pTcpSocket, &QTcpSocket::stateChanged, this, &CClientTcp::stateChanged);

} /* CClientTcp::initializationInNewThread */

void CClientTcp::socketReadyRead()
{
    if (!m_pTcpSocket)
        return;

    const QByteArray data = m_pTcpSocket->readAll();
    if (data.size() < 1)
        return;

    //    qDebug() << "RX: " << data.size() << data.toHex( ' ');

    emit receivedData(data);
} /* CClientTcp::socketReadyRead */

void CClientTcp::acceptError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    if (!m_pTcpSocket)
        return;

    qDebug() << Q_FUNC_INFO << m_pTcpSocket->errorString();
} /* CClientTcp::acceptError */

void CClientTcp::stateChanged(QAbstractSocket::SocketState socketState)
{
    //    qDebug() << Q_FUNC_INFO << socketState;
    //    emit stateConnectChanged( socketState == QAbstractSocket::ConnectedState);

    if (socketState == QAbstractSocket::ConnectedState)
    {
        if (m_isConnected == false)
        {
            m_isConnected = true;
            emit stateConnectChanged(true);
        } /* if */
    } /* if */
    else
    {
        if (m_isConnected == true)
        {
            m_isConnected = false;
            emit stateConnectChanged(false);
        } /* if */
    } /* else */
} /* CClientTcp::stateChanged */
