#include "lens_j200_controller.h"
#include <QDebug>
#include <execinfo.h>

LensJ200Controller::LensJ200Controller(QObject *parent)
    : QObject{parent}, m_isConnected(false)
{
    lensWorker.init();
    // API

    QObject::connect(this, &LensJ200Controller::lensConnect, &lensWorker,
                     &LensJ200Worker::onConnect, Qt::QueuedConnection);

    QObject::connect(this, &LensJ200Controller::lensSendRequest, &lensWorker,
                     &LensJ200Worker::onSendRequest, Qt::QueuedConnection);

    QObject::connect(&lensWorker, &LensJ200Worker::connected, this,
                     &LensJ200Controller::onConnected, Qt::QueuedConnection);

    // QObject::connect(&lensWorker, &LensJ200Worker::error, this, &LensJ200Wrapper::onError,
    // Qt::QueuedConnection);

} /* LensJ200Controller::LensJ200Controller */

void printStackTrace()
{
    void *callstack[128];
    int frames = backtrace(callstack, 128);
    char **strs = backtrace_symbols(callstack, frames);

    qDebug() << "Stack trace:";
    for (int i = 0; i < frames; ++i)
    {
        qDebug() << strs[i];
    } /* for */

    free(strs);
} /* printStackTrace */
void LensJ200Controller::connect(QString portname, quint32 baudrate)
{
    emit lensConnect(true, portname, baudrate);
} /* LensJ200Controller::connect */

void LensJ200Controller::proc_disconnect()
{
    qDebug() << Q_FUNC_INFO;
    emit lensConnect(false);
} /* LensJ200Controller::proc_disconnect */

void LensJ200Controller::onConnected(bool isConnected)
{
    qDebug() << Q_FUNC_INFO << "isConnected =" << isConnected;
    m_isConnected = isConnected;
    // printStackTrace();
} /* LensJ200Controller::onConnected */

void LensJ200Controller::proc_report()
{
    emit lensSendRequest(Lens::Command::Report);
} /* LensJ200Controller::proc_report */

void LensJ200Controller::proc_zoomOutContinuous()
{
    if (lensWorker.status().zoomPosition == Lens::minEncoderValue)
    {
        return;
    } /* if */
    emit lensSendRequest(Lens::Command::ZoomOutContinuous);
} /* LensJ200Controller::proc_zoomOutContinuous */

void LensJ200Controller::proc_zoomInContinuous()
{
    if (lensWorker.status().zoomPosition >= Lens::maxEncoderValue)
    {
        return;
    } /* if */
    emit lensSendRequest(Lens::Command::ZoomInContinuous);
} /* LensJ200Controller::proc_zoomInContinuous */

void LensJ200Controller::proc_zoomStop()
{
    emit lensSendRequest(Lens::Command::ZoomStop);
} /* LensJ200Controller::proc_zoomStop */

void LensJ200Controller::proc_focusFarContinuous()
{
    if (lensWorker.status().focusPosition == Lens::minEncoderValue)
    {
        return;
    } /* if */
    emit lensSendRequest(Lens::Command::FocusFarContinuous);
} /* LensJ200Controller::proc_focusFarContinuous */

void LensJ200Controller::proc_focusNearContinuous()
{
    if (lensWorker.status().focusPosition >= Lens::maxEncoderValue)
    {
        return;
    } /* if */
    emit lensSendRequest(Lens::Command::FocusNearContinuous);
} /* LensJ200Controller::proc_focusNearContinuous */

void LensJ200Controller::proc_focusStop()
{
    emit lensSendRequest(Lens::Command::FocusStop);
} /* LensJ200Controller::proc_focusStop */

void LensJ200Controller::proc_gotoPosition(quint16 zoomPos, quint16 focusPos)
{
    QByteArray args(4, '\0');
    args[0] = (quint8)zoomPos;
    args[1] = zoomPos >> 8;
    args[2] = (quint8)focusPos;
    args[3] = focusPos >> 8;
    emit lensSendRequest(Lens::Command::GotoPosition, args);
} /* LensJ200Controller::proc_gotoPosition */

void LensJ200Controller::proc_setSpeed(quint8 zoomSpeed, quint8 focusSpeed)
{
    QByteArray args(2, '\0');
    args[0] = zoomSpeed;
    args[1] = focusSpeed;

    emit lensSendRequest(Lens::Command::SpeedSet, args);
} /* LensJ200Controller::proc_setSpeed */

void LensJ200Controller::proc_getFocalLength()
{
    emit lensSendRequest(Lens::Command::GetFocalLength);
} /* LensJ200Controller::proc_getFocalLength */

void LensJ200Controller::proc_gotoFocalLength(quint16 focalLength)
{
    QByteArray args(2, '\0');
    args[0] = (quint8)focalLength;
    args[1] = focalLength >> 8;

    emit lensSendRequest(Lens::Command::GotoFocalLength, args);
} /* LensJ200Controller::proc_gotoFocalLength */
