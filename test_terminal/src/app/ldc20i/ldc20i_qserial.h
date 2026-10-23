#ifndef LDC20I_QSERIAL_H
#define LDC20I_QSERIAL_H

#include <QObject>
#include <QSerialPort>
#include <qthread.h>

class LDC20I_QSerial: public QObject
{
    Q_OBJECT

    QThread thread;

public:
    explicit LDC20I_QSerial(QString port, quint32 baudrate, QObject *parent = nullptr);
    ~LDC20I_QSerial();

private:
    QString port;
    quint32 baudrate;
    QSerialPort* serialPort;
    QByteArray buf;
public slots:
    qint32 initConnection();
    qint32 closeConnection();

    void serialReceived();
    void serialBytesWritten(qint64 bytes);
    void proc_sendData(QByteArray ba);
    void proc_Thread_Finished();

Q_SIGNALS:
    void dataParse(QByteArray ba);
    void deviceConnected();

};

#endif // LDC20I_QSERIAL_H
