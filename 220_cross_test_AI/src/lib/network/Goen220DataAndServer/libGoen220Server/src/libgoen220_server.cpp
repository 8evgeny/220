#include "libgoen220_server.h"
#include "_internal/server_impl.h"

#include <QAbstractEventDispatcher>
#include <QCoreApplication>
#include <QDebug>
#include <QUdpSocket>
#include <QThread>

namespace Goen220
{
    namespace Server
    {

        Intrf::Intrf(QObject *parent)
            : QObject{parent}
        {
        } /* Intrf::Intrf */

        Intrf::~Intrf()
        {
        } /* Intrf::~Intrf */

        Intrf *create(QObject *parent)
        {
            return new ServerImpl(parent);
        } /* create */

        Intrf *createOnThread(QThread *th)
        {
            if (QThread::currentThread() == th)
                return create();

            Intrf *s = nullptr;

            auto f = [&s]
            {
                s = create();
            } /* lambda */;

            if (th == nullptr)
                th = new QThread();

            if (!th->isRunning())
                th->start();

            { // wait for thread running
                QEventLoop ll;
                while (!th->isRunning())
                    ll.processEvents(QEventLoop::ExcludeUserInputEvents);
            } /* block */

            auto ctx = new QObject();
            ctx->moveToThread(th);
            QMetaObject::invokeMethod(ctx, f, Qt::QueuedConnection);
            ctx->deleteLater();

            { // wait for new Server
                QEventLoop ll;
                while (s == nullptr)
                    ll.processEvents(QEventLoop::ExcludeUserInputEvents);
            } /* block */

            return s;
        } /* createOnThread */

        void removeServer(Intrf *server)
        {
            QEventLoop ll;

            QObject::connect(
                server, &QObject::destroyed, &ll, [&ll]
                {
                    ll.exit();
                } /* lambda */,
                Qt::QueuedConnection);

            server->deleteLater();
            ll.exec();
        } /* removeServer */

    } // namespace Server
} // namespace Goen220
