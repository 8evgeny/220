#ifndef CAMERA_EV9500M_H
#define CAMERA_EV9500M_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QElapsedTimer>
#include "visca/libvisca.h"

// Состояние режима фокуса «неизвестно» (до первого опроса камеры).
#define TV_FOCUS_MODE_UNKNOWN        0x00
// Максимальное время ожидания Completion от камеры перед сбросом bIsBusy.
#define TV_VISCA_COMPLETION_TIMEOUT_MS 3000
#define TV_VISCA_BUSY_POLL_INTERVAL_MS  500

// Значения параметров протокола ГОЭН-220 (табл. 6, 1-OFF / 2-ON и т.п.)
#define EVCAM_MODE_OFF                  0x01
#define EVCAM_MODE_ON                   0x02
#define EVCAM_STABILIZER_HOLD           0x03
#define EVCAM_FOCUS_MODE_MANUAL         0x01
#define EVCAM_FOCUS_MODE_AUTO           0x02
#define EVCAM_AUTO_ICR_ON_COLOR         0x04

const uint8_t VISCA_POWER_ON = 2;
const uint8_t VISCA_POWER_OFF = 3;
const uint8_t VISCA_FOCUS_CHANGE = 0x10;

namespace VISCA {
const quint8 RGB = 1;
}

struct CameraStatus {
    qint8 temperature = 0;
    quint16 iris = 0;
    quint16 shutter = 0;
    quint16 gain = 0;
    quint8 contrast = 0;
    quint16 focus_val = 0;
    // Режим фокуса VISCA: 0x02 — Auto (АФ включён), 0x03 — Manual (АФ выключен).
    quint8 focus_mode = TV_FOCUS_MODE_UNKNOWN;
    quint16 zoom_val = 0;
    quint8 awb_display_mode = 0;
};

class TVCamera : public QObject
{
    Q_OBJECT
public:
    enum Command {
        ZoomTele, ZoomWide, ZoomStop, ZoomPos,
        FocusFar, FocusNear, FocusStop, FocusPos, FocusMode,
        FocusAutoOn, FocusAutoOff, FocusOnePush,
        AFModeNormal, AFModeInterval, AFModeZoomTrig,
        IrisOpen, IrisClose, IrisManual,
        AutoExpManual, AutoExpAuto,
        ShutterUp, ShutterDown, ShutterManual,
        GainUp, GainDown, GainManual,
        ContrastValue, ChangeBaudrate,
        DefogOn, DefogOff, SetNR, Set23DNR,
        DZoomOn, DZoomOff, DZoomLimit, DZoomMode,
        SpotFocusOn, SpotFocusOff, SpotFocusParam,
        CorrBlockIR, CorrPassIR,
        WBMode, WBTrigger,
        SpotAWBOn, SpotAWBOff, SpotAWBPar,
        SpotAWBDispOn, SpotAWBDispOff,
        SlowShutterAuto, SlowShutterManual,

        // Расширенные команды ТВ-камеры (табл. 6 протокола ГОЭН-220 v1.1)
        FlickerReduction, BackLight,
        GammaValue, GammaOffset,
        ExpCompReset, ExpCompUp, ExpCompDown, ExpCompDirect,
        StabilizerMode, StabilizerLevel,
        HighSensitivity, WideDMode, WideDParams,
        NrLevel, Nr2D3D, HlcParams,
        IcrMode, AutoIcrMode, AutoIcrThreshold,
        MinShutterMode, MinShutterLimit, PictureFlip
    };

    enum Baudrate {
        Br9600 = B9600,
        Br19200 = B19200,
        Br38400 = B38400,
        Br115200 = B115200
    };

    explicit TVCamera(QObject *parent = nullptr);
    ~TVCamera();
    int connect(QString portName, quint32 baudrate);
    void disconnect();

    CameraStatus* status() { return &m_status; }
    bool isConnected() { return bIsConnected; }
    bool isBusy() { return bIsBusy; }

    quint32 executeCommand(Command cmd, quint32 arg);
    void setAutoExpMode(Command cmd);
    void setBrightness(quint32 value);
    void getBrightness();
    bool setZoomValue(quint32 value);
    bool setFocusValue(quint32 value);
    void setVideoFormat(quint8 format);

    // ═══════════════════════════════════════════════════════════════
    //  Управление режимом фокуса по VISCA FCB-EV9500M
    //  Auto = 0x38 02, Manual (АФ выключен) = 0x38 03,
    //  переключатель = 0x38 10, One Push AF = 0x18 01.
    //  Ручные команды фокуса исполняются камерой ТОЛЬКО в Manual,
    //  поэтому перед ними выполняется принудительный переход в Manual.
    // ═══════════════════════════════════════════════════════════════
    bool setFocusModeAuto();
    bool setFocusModeManual();
    bool toggleFocusMode();
    bool getFocusMode();
    bool setAfMode(quint8 mode);
    quint8 focusMode() const { return m_status.focus_mode; }

    bool getTemperature();
    bool getIris();
    bool getShutter();
    bool getGain();
    bool getContrast();
    bool getFocusValue();
    bool getZoomValue();
    bool getAWBDisplayMode();

protected:
    // Гарантирует Manual перед ручным движением фокуса (иначе камера
    // отвечает Command Not Executable 90 6y 41 FF).
    bool ensureManualFocusMode();

    // Установка busy с запуском таймера ожидания Completion от камеры.
    void setBusy(bool busy);

    // busy запрещает только новые команды движения фокуса/зума.
    // Stop-команды должны проходить всегда, чтобы снимать busy.
    static bool isMotionCommand(Command cmd);

    VISCAInterface_t interface;
    VISCACamera_t camera;
    CameraStatus m_status;
    QTimer timer;
    QElapsedTimer m_busyTimer;

    bool bIsConnected;
    bool bIsBusy;
    // Желаемый режим фокуса: 0 — неизвестно, иначе VISCA_FOCUS_AUTO_ON/OFF.
    // Используется, чтобы не слать повторные 0x38 и не зависеть от toggle.
    quint8 m_desiredFocusMode = TV_FOCUS_MODE_UNKNOWN;
    quint32 errViscaCount;
    QString m_portName;
    QHash<quint32, Baudrate> baudrateHash;

signals:
    void sendStatus();

public slots:
    void process();

private slots:
    void timeout();
};

#endif // CAMERA_EV9500M_H
