#ifndef GyroPlatformVCD_QSERIAL_H
#define GyroPlatformVCD_QSERIAL_H

#include <QObject>
#include <QSerialPort>
#include <qthread.h>

class GyroPlatformVCD_QSerial: public QObject
{
    Q_OBJECT

    QThread thread;

public:
    explicit GyroPlatformVCD_QSerial(QString port, quint32 baudrate, QObject *parent = nullptr);

private:

    QString port = "";
    quint32 baudrate = 0;

    QSerialPort* m_serialPort = nullptr;

    QByteArray buf;
public slots:
    qint32 initConnection();
    qint32 closeConnection();

    void serialReceived();
    void serialBytesWritten(qint64 bytes);
    void proc_sendData(QByteArray ba);

Q_SIGNALS:
    void dataParse(QByteArray ba);
    void deviceConnected();
    void sig_READY();

};

#endif // GyroPlatformVCD_QSERIAL_H
