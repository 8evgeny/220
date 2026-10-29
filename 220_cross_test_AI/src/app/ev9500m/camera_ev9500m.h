#ifndef CAMERA_EV9500M_H
#define CAMERA_EV9500M_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "visca/libvisca.h"

const uint8_t VISCA_POWER_ON = 2;
const uint8_t VISCA_POWER_OFF = 3;
const uint8_t VISCA_FOCUS_CHANGE = 0x10;

namespace VISCA
{
    const quint8 RGB = 1;
} /* namespace VISCA */

struct CameraStatus
{
    qint8 temperature = 0;
    quint16 iris = 0;
    quint16 shutter = 0;
    quint16 gain = 0;
    quint8 contrast = 0;
    quint16 focus_val = 0;
    quint16 zoom_val = 0;
    quint8 awb_display_mode = 0;
} /* struct CameraStatus */;

class TVCamera : public QObject
{
    Q_OBJECT
  public:
    enum Command
    {
        ZoomTele,
        ZoomWide,
        ZoomStop,
        ZoomPos,
        FocusFar,
        FocusNear,
        FocusStop,
        FocusPos,
        FocusMode,
        IrisOpen,
        IrisClose,
        IrisManual,
        AutoExpManual,
        AutoExpAuto,
        ShutterUp,
        ShutterDown,
        ShutterManual,
        GainUp,
        GainDown,
        GainManual,
        ContrastValue,
        ChangeBaudrate,
        DefogOn,
        DefogOff,
        SetNR,
        Set23DNR,
        DZoomOn,
        DZoomOff,
        DZoomLimit,
        DZoomMode,
        SpotFocusOn,
        SpotFocusOff,
        SpotFocusParam,
        CorrBlockIR,
        CorrPassIR,
        WBMode,
        WBTrigger,
        SpotAWBOn,
        SpotAWBOff,
        SpotAWBPar,
        SpotAWBDispOn,
        SpotAWBDispOff,
        SlowShutterAuto,
        SlowShutterManual
    };

    enum Baudrate
    {
        Br9600 = B9600,
        Br19200 = B19200,
        Br38400 = B38400,
        Br115200 = B115200
    } /* enum Baudrate */;

    explicit TVCamera(QObject *parent = nullptr);
    ~TVCamera();

    int connect(QString portName, quint32 baudrate);

    void disconnect();

    CameraStatus *status()
    {
        return &m_status;
    } /* status */
    bool isConnected()
    {
        return bIsConnected;
    } /* isConnected */
    bool isBusy()
    {
        return bIsBusy;
    } /* isBusy */

    quint32 executeCommand(Command cmd, quint32 arg);
    void setAutoExpMode(Command cmd);
    void setBrightness(quint32 value);
    void getBrightness();
    bool setZoomValue(quint32 value);
    bool setFocusValue(quint32 value);
    void setVideoFormat(quint8 format);

    bool getTemperature();
    bool getIris();
    bool getShutter();
    bool getGain();
    bool getContrast();
    bool getFocusValue();
    bool getZoomValue();
    bool getAWBDisplayMode();

  protected:
    VISCAInterface_t interface;
    VISCACamera_t camera;
    CameraStatus m_status;
    QTimer timer;

    bool bIsConnected;
    bool bIsBusy;
    quint32 errViscaCount;
    QString m_portName;
    QHash<quint32, Baudrate> baudrateHash;

  signals:
    void sendStatus();

  public slots:
    void process();

  private slots:
    void timeout();
} /* class TVCamera */;

#endif // CAMERA_EV9500M_H
