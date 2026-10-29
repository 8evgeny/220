#ifndef LENS_J200_CONTROLLER_H
#define LENS_J200_CONTROLLER_H

#include <QObject>
#include "lens_j200_worker.h"

class LensJ200Controller : public QObject
{
    Q_OBJECT
  public:
    explicit LensJ200Controller(QObject *parent = nullptr);

    LensJ200Worker lensWorker;

    void connect(QString portName, quint32 baudrate = 19200);

    bool isConnected()
    {
        return lensWorker.status().isConnected;
    } /* isConnected */;
    qint8 getTemperature()
    {
        return lensWorker.status().temperature;
    } /* getTemperature */
    quint16 getZoomPosition()
    {
        return lensWorker.status().zoomPosition;
    } /* getZoomPosition */
    quint16 getFocusPosition()
    {
        return lensWorker.status().focusPosition;
    } /* getFocusPosition */
    quint16 getFocalLength()
    {
        return lensWorker.status().focalLength;
    } /* getFocalLength */
    Lens::Error error()
    {
        return lensWorker.error();
    } /* error */
    void proc_disconnect();
    void proc_report();
    void proc_zoomOutContinuous();
    void proc_zoomInContinuous();
    void proc_zoomStop();
    void proc_focusFarContinuous();
    void proc_focusNearContinuous();
    void proc_focusStop();
    void proc_gotoPosition(quint16 zoomPos, quint16 focusPos);
    void proc_setSpeed(quint8 zoomSpeed, quint8 focusSpeed);
    void proc_getFocalLength();
    void proc_gotoFocalLength(quint16 focalLength);

  protected:
    bool m_isConnected;

  signals:
    void lensConnect(bool isConnect, QString portName = "", quint32 baudrate = 19200);
    void lensDisconnect();
    void lensSendRequest(Lens::Command, const QByteArray &data = {});

  public slots:
    void onConnected(bool);
    // void onError(Lens::Error);

} /* class LensJ200Controller */;

#endif // LENS_J200_CONTROLLER_H
