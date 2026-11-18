#ifndef CNEWTHREAD_H
#define CNEWTHREAD_H

#include <QObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY( serviceThreadState)


namespace NCore
{


    class CNewThread : public QObject
    {
        Q_OBJECT

    public:
        explicit CNewThread( QObject *parent = nullptr);
        virtual ~CNewThread();

    public slots:
        void stopThread();

    protected slots:
        virtual void initializationInNewThread() = 0;

    private:
        QThread *m_pThread;

    };


}   //  namespace NCore

#endif // CNEWTHREAD_H
