#include "lens_j200_worker.h"
#include <QDebug>
//#define DEBUG
LensJ200Worker::LensJ200Worker(QObject *parent)
    : QObject{parent},
    requestCount(0),
    retryCount(0),
    m_isProcessed(false),
    m_error(Lens::Error::NoError)
{}

LensJ200Worker::~LensJ200Worker()
{
    qDebug() << Q_FUNC_INFO;
    qDebug()<<Q_FUNC_INFO << "Lens thread exit";
    thread.exit();
    thread.wait();
}

void LensJ200Worker::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
    timer->stop();
    delete timer;
    delete lens;
}


void LensJ200Worker::init()
{
    qDebug()<<Q_FUNC_INFO<<"Main thread id:"<<QThread::currentThreadId();
    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        qDebug() << Q_FUNC_INFO << "Lens thread started with id:" << QThread::currentThreadId();
        lens = new LensJ200;
        timer = new QTimer;
        timer->setSingleShot(true);
        QObject::connect(lens, &LensJ200::sendResponse, this, &LensJ200Worker::onReceivedAck);
        QObject::connect(timer, &QTimer::timeout, this, &LensJ200Worker::onTimerTimeout);
    });

    QObject::connect(&thread, &QThread::finished, this, &LensJ200Worker::proc_Thread_Finished);

    thread.start();
}

void LensJ200Worker::onConnect(bool isConnect, QString portName)
{
    // Disconnect
    if ( !isConnect ) {
        timer->stop();
        lens->close();
        emit connected(false);
        return;
    }
    // Connect
    if ( !lens->open(portName) ) {
        qDebug()<<"Error opening serial port:"<<portName;
        emit connected(false);
        timer->start();
        return;
    }

    m_error = Lens::Error::NoError;
    requestQueue.clear();
    m_isProcessed = false;
    emit connected(true);
}


void LensJ200Worker::onCloseConnection()
{
    // Disconnect
    lens->close();
    timer->stop();
    emit connected(false);
}



void LensJ200Worker::onSendRequest(Lens::Command commandID, const QByteArray& data)
{
    QByteArray request;
    request.append((quint8)commandID);
    request.append(data);

    //   qDebug()<<Q_FUNC_INFO<<"--------------"<<m_isProcessed;
    //  qDebug()<<Q_FUNC_INFO<<request.toHex(' ');

    requestQueue.enqueue(request);
    if ( !m_isProcessed ) {
        sendRequest();
        return;
    }

    //    lens->send(request);
}

void LensJ200Worker::sendRequest()
{
    m_isProcessed = true;
    m_request = requestQueue.dequeue();
    lens->send(m_request);
    requestCount++;
    timer->start(100);
}

void LensJ200Worker::onReceivedAck(bool ack)
{
    // qDebug()<<Q_FUNC_INFO<<"recved responses:"<<requestCount<<"ack ="<<ack<<retryCount;

    timer->stop();

    //There was a transmission error, repeat the request
    if ( !ack ) {
        if (retryCount++ >= maxRetryCount-1) {
            //emit error(Lens::Error::BadResponse);
            m_error = Lens::Error::BadResponse;
            requestQueue.clear();
            retryCount = 0;
            return;
        }
        lens->send(m_request);
        timer->start(100);
        return;
    }

    retryCount = 0;

    if ( requestQueue.isEmpty() ) {
        timer->stop();
        m_isProcessed = false;
        return;
    }
    sendRequest();
}

void LensJ200Worker::onTimerTimeout()
{
  //  qDebug()<<Q_FUNC_INFO<<"timeout error occured";
    //emit error(Lens::Error::Timeout);
    m_error = Lens::Error::Timeout;
    this->errorReset();
}
