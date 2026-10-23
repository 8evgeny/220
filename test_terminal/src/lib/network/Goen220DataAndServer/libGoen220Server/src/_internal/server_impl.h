#pragma once

#include <QObject>
#include <QUdpSocket>

#include "libgoen220_server.h"
#include "tcp_server.h"

namespace Goen220 {
namespace Server {

class ServerImpl : public Intrf
{
    Q_OBJECT

public:
    explicit ServerImpl(QObject *parent = nullptr);

    ~ServerImpl();

    void setError(Intrf::Error err, int errCode, const QString &errorStr);

    void onTcpServerError();

    void onTlmTimerTimeout();

    void onNewClient(QTcpSocket *socket);

    void onClientDisconnected(QTcpSocket *socket);

    void onSetRequest(const TcpServer::VdpHeader *h, const char *data);


    void onGetRequest(const TcpServer::VdpHeader *h);

    void onDataReceived(QTcpSocket *socket, const QByteArray &data);

    // Server interface
public:
    quint16 tcpPort() const override;
    bool isKeepAliveProcEnabled() const override;
    bool isClientConnected() const override;
    QHostAddress client() const override;
    quint16 tlmUdpPort() const override;
    int tlmTransmissionIntervalMs() const override;
    Tlm *tlm() const override;
    void setTlmSrc(Tlm *tlm) override;
    QString lastErrorString() const override;
    Error lastError() const override;
    int lastErrorCode() const override;

public slots:
    void setListening(bool state) override;
    void setKeepAliveProcEnabled(bool state) override;
    void setTcpPort(quint16 port) override;
    void setTlmUdpPort(quint16 port) override;
    void setTlmTransmissionIntervalMs(int ms) override;

    void cmdAck(Goen220::Cmd cmd, Goen220::CmdAck ack) override;

private:
    //------------------------------

    quint16 m_tcpPort = 5251;
    TcpServer m_tcp;
    quint16 m_tlmUdpPort = 5251;
    QTimer m_tlmTimer;

    Goen220::Tlm m_tlmData;
    Goen220::Tlm *m_tlm = &m_tlmData;

    QHostAddress m_clientAddr;
    QTcpSocket *m_client = nullptr;

    Intrf::Error m_lastError = Intrf::Error::None;
    int m_lastErrorCode = -1;
    QString m_lastErrorString;
};
} // namespace Server
} // namespace Goen220
