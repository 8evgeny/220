#include "server_impl.h"

#include <QThread>

namespace Goen220
{
    namespace Server
    {

        ServerImpl::ServerImpl(QObject *parent)
            : Intrf{parent}
        {
            m_tlmTimer.setInterval(40);

            connect(&m_tlmTimer, &QTimer::timeout, this, &ServerImpl::onTlmTimerTimeout);

            connect(&m_tcp, &TcpServer::errorOccured, this, &ServerImpl::onTcpServerError);

            connect(&m_tcp, &TcpServer::newClient, this, &ServerImpl::onNewClient);

            connect(&m_tcp, &TcpServer::clientDisconnected, this,
                    &ServerImpl::onClientDisconnected);

            connect(&m_tcp, &TcpServer::dataReceived, this, &ServerImpl::onDataReceived);

        } /* ServerImpl::ServerImpl */

        ServerImpl::~ServerImpl()
        {
        } /* ServerImpl::~ServerImpl */

        void ServerImpl::setError(Error err, int errCode, const QString &errorStr)
        {
            m_lastError = err;
            m_lastErrorCode = errCode;
            m_lastErrorString = errorStr;

            emit errorOccured(err);
        } /* ServerImpl::setError */

        void ServerImpl::onTcpServerError()
        {
            auto errSrc = m_tcp.lastErrorSrc();

            setError(((errSrc == TcpServer::ErrorSrc::Server) ? Intrf::Error::TcpServer
                                                              : Intrf::Error::TcpClient),
                     (int)m_tcp.lastError(), m_tcp.lastErrorString());
        } /* ServerImpl::onTcpServerError */

        void ServerImpl::onTlmTimerTimeout()
        {
            static QUdpSocket udp;

            if (m_client == nullptr)
                return;

            QByteArray ba(sizeof(TcpServer::VdpHeader) + sizeof(Goen220::Tlm), 0);
            auto *h = (TcpServer::VdpHeader *)ba.data();
            h->cmd = 0;
            h->status = 0;
            h->src.device = 0x02;
            h->src.function = 0xFE;
            h->dst.device = 0x00;
            h->dst.function = 0xFE;
            h->byteCount = sizeof(Goen220::Tlm);
            std::memcpy(ba.data() + sizeof(TcpServer::VdpHeader), m_tlm, sizeof(Goen220::Tlm));

            if (auto res = udp.writeDatagram(ba, m_clientAddr, m_tlmUdpPort); res < 0)
            {
                qCritical() << "Could not transmit telemetry data." << udp.errorString();
                setError(Intrf::Error::Udp, (int)udp.error(), udp.errorString());
            } /* if */
        } /* ServerImpl::onTlmTimerTimeout */

        void ServerImpl::onNewClient(QTcpSocket *socket)
        {
            m_client = socket;
            m_clientAddr = socket->peerAddress();
            m_tlmTimer.start();
            emit clientConnectionStateChanged(true);
        } /* ServerImpl::onNewClient */

        void ServerImpl::onClientDisconnected(QTcpSocket *socket)
        {
            m_client = nullptr;
            m_tlmTimer.stop();
            emit clientConnectionStateChanged(false);
        } /* ServerImpl::onClientDisconnected */

        void ServerImpl::onSetRequest(const TcpServer::VdpHeader *h, const char *data)
        {
            auto dev = ((qint16)h->dst.device << 8);
            auto cmd = (Goen220::Cmd)(dev + h->dst.function);
            auto bc = h->byteCount;
            QByteArray pld(data, bc);

            emit cmdReceived(cmd, pld, (h->status & 0x80));
        } /* ServerImpl::onSetRequest */

        void ServerImpl::onGetRequest(const TcpServer::VdpHeader *h)
        {
            // do nothing
        } /* ServerImpl::onGetRequest */

        void ServerImpl::onDataReceived(QTcpSocket *socket, const QByteArray &data)
        {
            if (socket != m_client)
                return;

            // qDebug() << "Data received from" << socket->peerAddress().toString() << ":" <<
            // data.toHex();

            auto h = (const TcpServer::VdpHeader *)data.constData();
            if (h->cmd == 0x00)
                onSetRequest(h, data.constData() + sizeof(TcpServer::VdpHeader));
            if (h->cmd == 0x01)
                onGetRequest(h);
        } /* ServerImpl::onDataReceived */

        quint16 ServerImpl::tcpPort() const
        {
            return m_tcpPort;
        } /* ServerImpl::tcpPort */

        bool ServerImpl::isKeepAliveProcEnabled() const
        {
            return m_tcp.isKeepAliveProcEnabled();
        } /* ServerImpl::isKeepAliveProcEnabled */

        bool ServerImpl::isClientConnected() const
        {
            return m_client != nullptr;
        } /* ServerImpl::isClientConnected */

        QHostAddress ServerImpl::client() const
        {
            return m_clientAddr;
        } /* ServerImpl::client */

        quint16 ServerImpl::tlmUdpPort() const
        {
            return m_tlmUdpPort;
        } /* ServerImpl::tlmUdpPort */

        int ServerImpl::tlmTransmissionIntervalMs() const
        {
            return m_tlmTimer.interval();
        } /* ServerImpl::tlmTransmissionIntervalMs */

        Tlm *ServerImpl::tlm() const
        {
            return m_tlm;
        } /* ServerImpl::tlm */

        void ServerImpl::setTlmSrc(Tlm *tlm)
        {
            if (tlm != nullptr)
                m_tlm = tlm;
            else
                m_tlm = &m_tlmData;
        } /* ServerImpl::setTlmSrc */

        QString ServerImpl::lastErrorString() const
        {
            return m_lastErrorString;
        } /* ServerImpl::lastErrorString */

        Intrf::Error ServerImpl::lastError() const
        {
            return m_lastError;
        } /* ServerImpl::lastError */

        int ServerImpl::lastErrorCode() const
        {
            return m_lastErrorCode;
        } /* ServerImpl::lastErrorCode */

        void ServerImpl::setListening(bool state)
        {
            auto f = [this](bool state)
            {
                if (state)
                    m_tcp.start(m_tcpPort);
                else
                    m_tcp.stop();
            } /* lambda */;

            if (QThread::currentThread() == thread())
                f(state);
            else
                QMetaObject::invokeMethod(this, f, Qt::QueuedConnection, state);
        } /* ServerImpl::setListening */

        void ServerImpl::setKeepAliveProcEnabled(bool state)
        {
            auto f = [this](bool state)
            {
                m_tcp.setKeepAliveProcEnabled(state);
            } /* lambda */;

            if (QThread::currentThread() == thread())
                f(state);
            else
                QMetaObject::invokeMethod(this, f, Qt::QueuedConnection, state);
        } /* ServerImpl::setKeepAliveProcEnabled */

        void ServerImpl::setTcpPort(quint16 port)
        {
            m_tcpPort = port;
        } /* ServerImpl::setTcpPort */

        void ServerImpl::setTlmUdpPort(quint16 port)
        {
            m_tlmUdpPort = port;
        } /* ServerImpl::setTlmUdpPort */

        void ServerImpl::setTlmTransmissionIntervalMs(int ms)
        {
            m_tlmTimer.setInterval(ms);
        } /* ServerImpl::setTlmTransmissionIntervalMs */

        void ServerImpl::cmdAck(Cmd cmd, CmdAck ack)
        {
            auto f = [this](Cmd cmd, CmdAck ack)
            {
                if (m_client == nullptr)
                    return;

                QByteArray ba(sizeof(TcpServer::VdpHeader), 0);
                auto *h = (TcpServer::VdpHeader *)ba.data();
                h->cmd = 0x80; // set response
                h->status = (quint8)ack; // acknowledge
                h->src.function = (quint8)cmd;
                h->src.device = ((quint16)cmd >> 8);
                h->dst.function = 0x00;
                h->dst.device = (quint8)((quint16)Goen220::Dev::Workstation >> 8);
                h->byteCount = 0;

                m_client->write(ba);
                m_client->flush();
            } /* lambda */;

            if (QThread::currentThread() == thread())
                f(cmd, ack);
            else
                QMetaObject::invokeMethod(this, f, Qt::QueuedConnection, cmd, ack);
        } /* ServerImpl::cmdAck */

    } // namespace Server
} // namespace Goen220
