
#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H


#include "CNewThread.h"


class QSerialPort;


class SerialPort : public NCore::CNewThread
{
    Q_OBJECT

public:
    explicit SerialPort( QObject *parent = nullptr);
    ~SerialPort();

    void setPortName( const QString &name);
    QString portName() const;

    static const QList<QString> getAvailablePorts();

public slots:
    void connect( const bool con);
    void sendData( const QByteArray data);

signals:
    void stateConnectChanged( const bool con);
    void errorOccurred( const QString error);
    void receivedData( const QByteArray data);



protected slots:
    void initializationInNewThread()    override;

    void readyRead( void);

private:
    QSerialPort     *m_pSerialPort;

    QString         m_portName;
    qint32          m_portBaudRate;

    bool            m_connected;
};


#endif

