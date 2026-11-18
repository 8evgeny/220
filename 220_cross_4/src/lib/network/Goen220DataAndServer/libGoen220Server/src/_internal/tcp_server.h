#pragma once

#include <QDateTime>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class TcpServer : public QTcpServer
{
    Q_OBJECT

    struct ClientData {
        qint64 m_lastKeepAliveTimestamp;

        ClientData()
            : m_lastKeepAliveTimestamp {QDateTime::currentMSecsSinceEpoch()}
        {}

        bool isAlive(qint64 timeoutMs = 2000) const
        {
            return (QDateTime::currentMSecsSinceEpoch() - m_lastKeepAliveTimestamp) < timeoutMs;
        }

        void setAlive()
        {
            m_lastKeepAliveTimestamp = QDateTime::currentMSecsSinceEpoch();
        }

    };

public:

#pragma pack(push, 1)
    struct VdpHeader {
        quint8 cmd;
        quint8 status;
        struct {
            quint8 function;
            quint8 device;
        } src;
        struct {
            quint8 function;
            quint8 device;
        }dst;
        quint16 byteCount;
    };
#pragma pack(pop)

    enum class ErrorSrc { None, Server, Client };

    explicit TcpServer(QObject *parent = nullptr);
    virtual ~TcpServer();

    bool isKeepAliveProcEnabled() const {return m_keepAliveProcEnabled;}

    void start(quint16 port);
    void stop();

    const QString& lastErrorString();
    QAbstractSocket::SocketError lastError() const;
    ErrorSrc lastErrorSrc() const;

signals:
    void newClient(QTcpSocket *socket);
    void clientDisconnected(QTcpSocket *socket);
    void dataReceived(QTcpSocket *socket, const QByteArray &data);

    void errorOccured();


public slots:

    void setKeepAliveProcEnabled(bool state);
    void sendData(QTcpSocket *socket, const QByteArray &data);
    void disconnectClient(QTcpSocket *socket);


private slots:

    void onKeepAliveTimerTimeout();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:

    void onClientReadyRead();
    void onClientDisconnected();
    void onClientErrorOccurred(QAbstractSocket::SocketError error);

private:
    QHash<QTcpSocket *, ClientData> m_clients;
    bool m_keepAliveProcEnabled = true;
    QTimer m_keepAliveTimer;
    ErrorSrc m_lastErrSrc = ErrorSrc::None;
    QAbstractSocket::SocketError m_lastErr = QAbstractSocket::SocketError::UnknownSocketError;
    QString m_lastErrStr;
};
