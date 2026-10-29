#ifndef CSERVERTCP_H
#define CSERVERTCP_H

#include "CNewThread.h"

#include <QObject>
#include <QAbstractSocket>
#include <QLoggingCategory>


class QTcpServer;
class QTcpSocket;


Q_DECLARE_LOGGING_CATEGORY( serviceTcpServerState)
Q_DECLARE_LOGGING_CATEGORY( serviceTcpServerTx)
Q_DECLARE_LOGGING_CATEGORY( serviceTcpServerRx)


namespace NNetwork
{


    class CServerTcp : public NCore::CNewThread
    {
        Q_OBJECT
    public:
        explicit CServerTcp( const unsigned short port, const unsigned char nodeId, QObject *parent = nullptr);
        ~CServerTcp();

        unsigned char getNodeId() const     {   return m_nodeId;    }

    signals:
        void stateClientChanged( const unsigned char nodeId, const bool con);
        void receivedData( const QByteArray data);

        void serverError( const QString text);

    public slots:
        void sendData( const QByteArray data);

    protected slots:
        void initializationInNewThread() override;

    private slots:
        void newConnectionIsAvailable();
        void acceptError( QAbstractSocket::SocketError socketError);
        void socketReadyRead();
        void socketDisconnected();

    private:
        const unsigned short m_port;
        const unsigned char m_nodeId;

        QTcpServer *m_pTcpServer;
        QTcpSocket *m_pTcpSocket;

    };


}   //  namespace NNetwork

#endif // CSERVERTCP_H
