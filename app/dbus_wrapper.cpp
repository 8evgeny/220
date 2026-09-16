#include "dbus_wrapper.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>

DBUS_Wrapper::DBUS_Wrapper(SettingsManager *sm, QObject *parent)
    : QObject(parent)
    , sm {sm}

{
    qDebug() << Q_FUNC_INFO<< "GST_Wrapper thread ";

    moveToThread(&thread);
    // connect(&thread, &QThread::started, this, [this]{
    //     dbus_CommInit();

    // }, Qt::DirectConnection);

    thread.start();


}

#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>


    // Method: SetBrightness
    void DBUS_Wrapper::setBrightness(double brightness) {
        if(!isInit)
            return;
        QDBusReply<void> reply = dbusInterface->call("SetBrightness", brightness);
        if (reply.isValid()) {
            qDebug() << "SetBrightness called successfully.";
        } else {
            qWarning() << "Failed to call SetBrightness:" << reply.error().message();
        }
    }

    // Method: GetOutObject
    void DBUS_Wrapper::getOutObject(uint &cx_out, uint &cy_out, uint &w_out, uint &h_out) {
        if(!isInit)
            return;
        QDBusMessage reply = dbusInterface->call("GetOutObject");
        if (reply.type() == QDBusMessage::ReplyMessage) {
            QList<QVariant> args = reply.arguments();
            if (args.size() == 4) {
                uint cx = args[0].toUInt();
                uint cy = args[1].toUInt();
                uint w = args[2].toUInt();
                uint h = args[3].toUInt();
                cx_out = cx;
                cy_out = cy;
                w_out = w;
                h_out = h;
               // qDebug() << "GetOutObject returned:" << cx << cy << w << h;
            } else {
                qWarning() << "Unexpected number of arguments returned by GetOutObject.";
            }
        } else {
            qWarning() << "Failed to call GetOutObject:" << reply.errorMessage();
        }
    }

    // Method: GetOutRoi
    void DBUS_Wrapper::getOutRoi(uint &cx_out, uint &cy_out, uint &w_out, uint &h_out) {
        if(!isInit)
            return;
        QDBusMessage reply = dbusInterface->call("GetOutRoi");
        if (reply.type() == QDBusMessage::ReplyMessage) {
            QList<QVariant> args = reply.arguments();
            if (args.size() == 4) {
                uint cx = args[0].toUInt();
                uint cy = args[1].toUInt();
                uint w = args[2].toUInt();
                uint h = args[3].toUInt();
                cx_out = cx;
                cy_out = cy;
                w_out = w;
                h_out = h;
               qDebug() << "GetOutRoi returned:" << cx << cy << w << h;
            } else {
                qWarning() << "Unexpected number of arguments returned by GetOutRoi.";
            }
        } else {
            qWarning() << "Failed to call GetOutRoi:" << reply.errorMessage();
        }
    }

    // Method: SetTargetObject
    void DBUS_Wrapper::setTargetObject(uint cx, uint cy, uint w, uint h) {
        if(!isInit)
            return;
        QDBusReply<void> reply = dbusInterface->call("SetTargetObject", cx, cy, w, h);
        qDebug() << "SetTargetObject set:" << 1 << cy << w << h;

        if (reply.isValid()) {

            qDebug() << "SetTargetObject called successfully.";
        } else {
            qWarning() << "Failed to call SetTargetObject:" << reply.error().message();
        }
    }

    // Method: SetStopTr
    void DBUS_Wrapper::setStopTracking() {
        if(!isInit)
            return;
        QDBusReply<void> reply = dbusInterface->call("SetStopTracking");
        if (reply.isValid()) {
            qDebug() << "SetStopTracking called successfully.";
        } else {
            qWarning() << "Failed to call SetStopTracking:" << reply.error().message();
        }
    }

    // Method: GetATg
    void DBUS_Wrapper::getActiveLockTracking(bool &active_out, bool &tracking_out) {
        if(!isInit)
            return;
        QDBusMessage reply = dbusInterface->call("GetActiveLockTracking");
        if (reply.type() == QDBusMessage::ReplyMessage) {
            QList<QVariant> args = reply.arguments();
            if (args.size() == 2) {
                bool active = args[0].toBool();
                bool locked = args[1].toBool();
                active_out = active;
                tracking_out = locked;

               // qDebug() << "GetActiveLockTracking returned:" << active << locked;
            } else {
                qWarning() << "Unexpected number of arguments returned by GetATg.";
            }
        } else {
            qWarning() << "Failed to call GetATg:" << reply.errorMessage();
        }
    }

    // Method: SetRExt
    void DBUS_Wrapper::setRoiExt(double ext) {
        if(!isInit)
            return;
        QDBusReply<void> reply = dbusInterface->call("SetRExt", ext);
        if (reply.isValid()) {
            qDebug() << "SetRExt called successfully.";
        } else {
            qWarning() << "Failed to call SetRExt:" << reply.error().message();
        }
    }



void DBUS_Wrapper::dbus_CommInit()
{
     QString serviceName = "org.istok.GoenTrack";
     QString objectPath = "/org/istok/GoenTrack";
     QString interfaceName = "org.istok.GoenTrack";
      dbusInterface = new QDBusInterface(serviceName, objectPath, interfaceName, QDBusConnection::sessionBus());
     if (!dbusInterface->isValid()) {
         qDebug() << "Failed to connect to D-Bus service.";
     }
     else
     isInit = true;

}
