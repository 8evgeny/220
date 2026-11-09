#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "ev9500m/camera_ev9500m_worker.h"
#include <QObject>
// #include "ips_bayer.h"
// #include "ips_ocl.h"
#include "dbus_wrapper.h"
#include "gst_wrapper.h"
#include "ldc20i/ldc20i_worker.h"
#include "mini640/mini640_worker.h"
#include "network.h"
#include "settingsmanager.h"
// #include "v4l2.h"
// #include <CL/cl2.hpp>
#include <qcoreapplication.h>
#include <qtimer.h>

#ifdef GYRO_PLATFORM_HOST
#include "Goen220GyroPlatformHost.h"
#include "gpf/GyroPlatformHandler.h"
#endif

class Main_app : public QObject
{
    Q_OBJECT
  public:
    explicit Main_app(QCoreApplication *app, QObject *parent = nullptr);
    ~Main_app();
    LDC20I_Worker *ldc_worker = nullptr;
    MINI640_Worker *mini640_worker = nullptr;
    LensJ200Wrapper *lens_j200_wrapper = nullptr;
    Camera_EV9500M_Worker *cw = nullptr;
    ;
    Network *net_conn = nullptr;
    GST_Wrapper *gst_wrp = nullptr;
    SettingsManager *sm = nullptr;

#ifdef GYRO_PLATFORM_HOST
    // Единая точка входа для завершения драйвера гироплатформы (принцип «единого окна»):
    // скрывает внутренности (m_pGyroPlatform / m_pGyroPlatformHandler) и позволяет
    // внешнему коду (в т.ч. обработчику сигналов) детерминированно освободить UART.
    void deinitGyroPlatform();
#endif

  private:
    void run_opencl();

    QCoreApplication *m_QT_App;

    //  IPS_Bayer* ips_bayer;
    //  IPS_Bayer* ips_bayer2;

    //   IPS_OCL* ips_ocl;
    //   IPS_OCL* ips_ocl2;

    // imageFormat_t tv_imageFormat;
    //  imageFormat_t ir_imageFormat;

    // V4L2 tv_v4l2;
    // V4L2 ir_v4l2;

    //  DBUS_Wrapper dbus_wrapper;
    QTimer video_init_timer;
    // QTimer ir_video_timer;

    // Подключение EV9500M (UART TV-камеры) разрешается только когда MPEG-TS пайп
    // поднят с реальной TV-камерой (GST_Wrapper::sig_RealTvCameraUp). При 10 неудачах
    // UART в режиме реальной камеры — рестарт пайпа по этому single-shot таймеру (5 с).
    bool m_ev9500mConnectInFlight = false;
    QTimer m_ev9500mRecoveryTimer;

    qint64 starttime;

    QMutex mutex_video_wait;
    bool isTvUartInit;
    bool isIRUartInit;

#ifdef GYRO_PLATFORM_HOST
    Goen220GyroPlatformHost *m_pGyroPlatform;
    GyroPlatformHandler *m_pGyroPlatformHandler;

    void initGyroPlatform();

#endif

  private slots:
    void proc_video_init_timeout();
    // void ir_video_timeout();

    // ─── EV9500M (UART TV-камеры): гейт запуска + восстановление ───────────
    // Подключение стартует только когда MPEG-TS пайп поднят с реальной TV-камерой
    // (GST_Wrapper::sig_RealTvCameraUp). В videotestsrc-fallback сигнал не приходит.
    void proc_ev9500mGate();             // по sig_RealTvCameraUp → разрешаем connect
    void proc_ev9500mConnected(bool ok); // по cw->sig_connected → при неудаче заводим recovery
    void proc_ev9500mRecoveryTimeout();  // по m_ev9500mRecoveryTimer → рестарт MPEG-TS пайпа

  public slots:
    // void video_reset_wd();
    // void ir_video_reset_wd();
    void proc_Switch_Channel(quint32 channel, quint8 en);
    void proc_MINI640_InitReady();
    void proc_GST_InitReady();
    void proc_deinitGyroPlatform();
    void proc_GST_RDY();

  signals:
    void sig_VISCA_CONNECT(QString portName, quint32 baudrate);
    void sig_VISCA_DISCONNECT();

    void sig_VISCA_CMD(TVCamera::Command cmd, quint32 value);

    void sig_MINI640_InitConnection();
    void sig_MINI640_CloseConnection();

    void sig_LensJ200_CloseConnection();
    void sig_LDC_CloseConnection();
    void sig_GSP_CloseConnection();

    void sig_GST_StopCapture();
    // Рестарт MPEG-TS encode-конвейера после сбоя UART EV9500M. Подключён к
    // GST_Wrapper::restartMpegTsPipeline с QueuedConnection (исполнится в worker-потоке).
    void sig_GST_RestartMpegTs();
}; // class Main_app

#endif // MAIN_APP_H
