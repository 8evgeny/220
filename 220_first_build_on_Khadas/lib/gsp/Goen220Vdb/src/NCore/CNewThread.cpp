#include "CNewThread.h"

#include <QThread>
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>
#include <QDateTime>


Q_LOGGING_CATEGORY( serviceThreadState, "service.thread.state")


using namespace NCore;


CNewThread::CNewThread( QObject *parent):
    QObject( parent)
{
    m_pThread = new QThread();
    this->moveToThread( m_pThread);
    m_pThread->start();

//    while( m_pThread->isRunning() == false)
//        QCoreApplication::processEvents();

//    QLoggingCategory testCat( "test=123;tes=567");
//    qCDebug( testCat) << "Started new thread. " << m_pThread;

//    qDebug() << "Started new thread. " << m_pThread;
//    qCInfo( serviceThreadState) << tr( "Started new thread. Address: 0x%1").arg(
//                                            QString::number( (unsigned long long)m_pThread, 16));

//    QTimer::singleShot( 100, this, &CNewThread::initializationInNewThread);
    connect( m_pThread, &QThread::started, this, &CNewThread::initializationInNewThread);
}


CNewThread::~CNewThread()
{
    if( m_pThread)
    {
        if( m_pThread->isRunning() == true)
        {
//            qDebug() << "Stop thread. " << m_pThread;
            qCInfo( serviceThreadState) << tr( "Stop thread. Address: 0x%1").arg(
                                                    QString::number( (unsigned long long)m_pThread, 16));
            m_pThread->quit();

            const qint64 waitTime = QDateTime::currentMSecsSinceEpoch() + 1000;

            do
            {
                QCoreApplication::processEvents();
            }
            while( m_pThread->isRunning() && QDateTime::currentMSecsSinceEpoch() < waitTime);
        }

//        qDebug() << "Delete thread. " << m_pThread;
        qCInfo( serviceThreadState) << tr( "Delete thread. Address: 0x%1").arg(
                                                QString::number( (unsigned long long)m_pThread, 16));
        m_pThread->deleteLater();
        m_pThread = nullptr;
    }
}


void CNewThread::stopThread()
{
    if( m_pThread)
    {
        if( m_pThread->isRunning() == true)
        {
//            qDebug() << "Stop thread. " << m_pThread;
            qCInfo( serviceThreadState) << tr( "Stop thread. Address: 0x%1").arg(
                                                    QString::number( (unsigned long long)m_pThread, 16));
            m_pThread->quit();
            if( m_pThread->wait( 1000) == false)
            {
//                qWarning() << "Terminate thread. " << m_pThread;
                qCInfo( serviceThreadState) << tr( "Terminate thread. Address: 0x%1").arg(
                                                        QString::number( (unsigned long long)m_pThread, 16));
                m_pThread->terminate();
            }
        }

//        qDebug() << "Delete thread. " << m_pThread;
        qCInfo( serviceThreadState) << tr( "Delete thread. Address: 0x%1").arg(
                                                QString::number( (unsigned long long)m_pThread, 16));
        m_pThread->deleteLater();
        m_pThread = nullptr;
    }
}
