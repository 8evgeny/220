#include "lens_j200_controller.h"
#include <QDebug>
#include <execinfo.h>

LensJ200Controller::LensJ200Controller(QObject *parent)
    : QObject{parent},
    m_isConnected(false)
{
    lensWorker.init();
    // API
    QObject::connect(this, &LensJ200Controller::lensConnect, &lensWorker, &LensJ200Worker::onConnect, Qt::QueuedConnection);
    QObject::connect(this, &LensJ200Controller::lensSendRequest, &lensWorker, &LensJ200Worker::onSendRequest, Qt::QueuedConnection);
    QObject::connect(&lensWorker, &LensJ200Worker::connected, this, &LensJ200Controller::onConnected, Qt::QueuedConnection);
    //QObject::connect(&lensWorker, &LensJ200Worker::error, this, &LensJ200Wrapper::onError, Qt::QueuedConnection);

}


void printStackTrace() {
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);

    qDebug() << "Stack trace:";
    for (int i = 0; i < frames; ++i) {
        qDebug() << strs[i];
    }

    free(strs);
}

void LensJ200Controller::connect(QString portname)
{
    emit lensConnect(true, portname);
}

void LensJ200Controller::proc_disconnect()
{
    qDebug()<<Q_FUNC_INFO;
    emit lensConnect(false);
}

void LensJ200Controller::onConnected(bool isConnected)
{
    qDebug()<<Q_FUNC_INFO<<"isConnected ="<<isConnected;
    m_isConnected = isConnected;
   // printStackTrace();
}

void LensJ200Controller::proc_report()
{
    emit lensSendRequest(Lens::Command::Report);
}

void LensJ200Controller::proc_zoomOutContinuous()
{
    if ( lensWorker.status().zoomPosition == Lens::minEncoderValue ) {
        return;
    }
    emit lensSendRequest(Lens::Command::ZoomOutContinuous);
}

void LensJ200Controller::proc_zoomInContinuous()
{
    if ( lensWorker.status().zoomPosition >= Lens::maxEncoderValue ) {
        return;
    }
    emit lensSendRequest(Lens::Command::ZoomInContinuous);
}

void LensJ200Controller::proc_zoomStop()
{
    emit lensSendRequest(Lens::Command::ZoomStop);
}

void LensJ200Controller::proc_focusFarContinuous()
{
    if ( lensWorker.status().focusPosition == Lens::minEncoderValue ) {
        return;
    }
    emit lensSendRequest(Lens::Command::FocusFarContinuous);
}

void LensJ200Controller::proc_focusNearContinuous()
{
    if ( lensWorker.status().focusPosition >= Lens::maxEncoderValue ) {
        return;
    }
    emit lensSendRequest(Lens::Command::FocusNearContinuous);
}

void LensJ200Controller::proc_focusStop()
{
    emit lensSendRequest(Lens::Command::FocusStop);
}

void LensJ200Controller::proc_gotoPosition(quint16 zoomPos, quint16 focusPos)
{
    QByteArray args(4, '\0');
    args[0] = (quint8)zoomPos;
    args[1] = zoomPos >> 8;
    args[2] = (quint8)focusPos;
    args[3] = focusPos >> 8;
    emit lensSendRequest(Lens::Command::GotoPosition, args);
}

void LensJ200Controller::proc_setSpeed(quint8 zoomSpeed, quint8 focusSpeed)
{
    QByteArray args(2, '\0');
    args[0] = zoomSpeed;
    args[1] = focusSpeed;

    emit lensSendRequest(Lens::Command::SpeedSet, args);
}

void LensJ200Controller::proc_getFocalLength()
{
    emit lensSendRequest(Lens::Command::GetFocalLength);
}

void LensJ200Controller::proc_gotoFocalLength(quint16 focalLength)
{
    QByteArray args(2, '\0');
    args[0] = (quint8)focalLength;
    args[1] = focalLength >> 8;

    emit lensSendRequest(Lens::Command::GotoFocalLength, args);
}
