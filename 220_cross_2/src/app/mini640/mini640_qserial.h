#ifndef MINI640_QSERIAL_H
#define MINI640_QSERIAL_H

#include <QObject>
#include <QSerialPort>
#include <qthread.h>

class MINI640_QSerial: public QObject
{
    Q_OBJECT

    QThread thread;

public:
    explicit MINI640_QSerial(QString port, quint32 baudrate, QObject *parent = nullptr);
    ~MINI640_QSerial();
    QSerialPort* m_serialPort = nullptr;
private:

    QString port = "";
    quint32 baudrate = 0;



    QByteArray buf;
public slots:
    qint32 initConnection();
    qint32 closeConnection();

    void serialReceived();
    void serialError(QSerialPort::SerialPortError error);

    void serialBytesWritten(qint64 bytes);
    void proc_sendData(QByteArray ba);
    void proc_Thread_Finished();

Q_SIGNALS:
    void dataParse(QByteArray ba);
    void deviceConnected();
    void sig_READY();

};

#endif // MINI640_QSERIAL_H
