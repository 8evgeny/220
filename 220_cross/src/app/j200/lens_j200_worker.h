#ifndef LENS_J200_WORKER_H
#define LENS_J200_WORKER_H

#include "lens_j200.h"
#include <QObject>
#include <QThread>
#include <QQueue>
#include <QTimer>


class LensProtocol : public QObject
{
    int command;
    int length;
    QByteArray data;
};

class LensJ200Worker : public QObject
{
    Q_OBJECT
public:
    explicit LensJ200Worker(QObject *parent = nullptr);
    ~LensJ200Worker();
    void init();
    LensStatus status() {
        return lens->status();
    }

    Lens::Error error() {
        return m_error;
    }

    void errorReset() {
        m_error = Lens::Error::NoError;
        requestQueue.clear();
        m_isProcessed = false;
    }

    LensProtocol message;

protected:
    QThread thread;
    LensJ200 *lens;
    QTimer *timer;
    QQueue<QByteArray> requestQueue;
    QByteArray m_request;
    quint8 retryCount;
    bool m_isProcessed;
    int requestCount;
    Lens::Error m_error;
    void sendRequest();

private:
    static const quint8 maxRetryCount = 5;

signals:
    void connected(bool);
//    void error(Lens::Error);

public slots:
    void onConnect(bool isConnect, QString portName);
    void onCloseConnection();

    void onSendRequest(Lens::Command commandID, const QByteArray& data = {});
    void onReceivedAck(bool);
    void onTimerTimeout();
    void proc_Thread_Finished();
};

#endif // LENS_J200_WORKER_H
