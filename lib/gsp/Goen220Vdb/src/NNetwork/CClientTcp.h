#ifndef CCLIENTTCP_H
#define CCLIENTTCP_H

#include <QObject>
#include <QAbstractSocket>

#include "CNewThread.h"


class QTcpSocket;


namespace NNetwork
{


    class CClientTcp : public NCore::CNewThread
    {
        Q_OBJECT

        Q_PROPERTY( QString hostName READ getHostName WRITE setHostName)
        Q_PROPERTY( unsigned short hostPort READ getHostPort WRITE setHostPort)

    public:
        explicit CClientTcp( QObject *parent = nullptr);

        QString getHostName() const						{	return m_hostName;		}
        void setHostName( const QString &name)			{	m_hostName = name;		}

        unsigned short getHostPort() const				{	return m_hostPort;		}
        void setHostPort( const unsigned short port)	{	m_hostPort = port;		}

    signals:
        void stateConnectChanged( const bool con);
        void receivedData( const QByteArray data);

    public slots:
        void connectToServer( const bool con);
        void sendData( const QByteArray data);

    protected slots:
        void initializationInNewThread() override;

    private slots:
        void socketReadyRead();
        void acceptError( QAbstractSocket::SocketError socketError);
        void stateChanged( QAbstractSocket::SocketState socketState);

    private:
        QTcpSocket *m_pTcpSocket;

        QString m_hostName;
        unsigned short m_hostPort;

        bool m_isConnected;

    };


}	//	namespace NNetwork

#endif // CCLIENTTCP_H
