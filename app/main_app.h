#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <QObject>
#include "ev9500m/camera_ev9500m_worker.h"
//#include "ips_bayer.h"
//#include "ips_ocl.h"
#include "dbus_wrapper.h"
#include "gst_wrapper.h"
#include "ldc20i/ldc20i_worker.h"
#include "mini640/mini640_worker.h"
#include "network.h"
#include "settingsmanager.h"
//#include "v4l2.h"
//#include <CL/cl2.hpp>
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
    LDC20I_Worker* ldc_worker  = nullptr;
    MINI640_Worker *mini640_worker = nullptr;
    LensJ200Wrapper *lens_j200_wrapper  = nullptr;
    Camera_EV9500M_Worker *cw;
    Network* net_conn = nullptr;
    GST_Wrapper* gst_wrp = nullptr ;

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


    SettingsManager* sm = nullptr;
  //  DBUS_Wrapper dbus_wrapper;
    QTimer tv_video_timer;
    QTimer ir_video_timer;

    qint64 starttime;

    QMutex mutex_video_wait;
    bool isTvUartInit;
    bool isIRUartInit;

#ifdef GYRO_PLATFORM_HOST
    Goen220GyroPlatformHost     *m_pGyroPlatform;
    GyroPlatformHandler         *m_pGyroPlatformHandler;

    void initGyroPlatform();

#endif



private slots:
    void tv_video_timeout();
    void ir_video_timeout();

public slots:
    void tv_video_reset_wd();
    void ir_video_reset_wd();
    void proc_Switch_Channel(quint32 channel, quint8 en);
    void proc_MINI640_InitReady();
    void proc_GST_InitReady();
    void proc_deinitGyroPlatform();

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
};

#endif // MAIN_APP_H
