#ifndef NETWORK_H
#define NETWORK_H
#include "gst_wrapper.h"
#include "telemetry.h"
#include <QObject>
#include <QUdpSocket>
#include <QQueue>
#include <QVariant>
#include <qthread.h>
//#include "v4l2.h"
#include "cmd_parser.h"
#include "settingsmanager.h"

class Network: public QObject
{

    Q_OBJECT
    QThread *thread;
public:
    explicit Network( SettingsManager *sm, GST_Wrapper *gst_wrapper, QObject *parent = nullptr);
    ~Network();
    void initSocket();
    CMD_Parser cmd_parser;

public slots:
   // void tvframeReady(frame_t sFrame);
   // void irframeReady(frame_t sFrame);

    void frameSend();
    void procVICAPstatus(CameraStatus status);
    void procLDCstatus(LDC_STATE dev_state);
    void procJ200status(J200_DEVSTATE ds);


    void procUpdateTrackerObject(ushort x,ushort y, ushort width, ushort heigth);
    void procUpdateActiveLockTracking(bool active, bool tracking);


    void proc_thread_finished();
    void proc_MINI640_State(MINI640_DEV_STATE dev_state);


    void procGspStatus( const bool connected);
    void procGspPositionMode(   const bool zero, const bool pohod, const bool pilot, const bool park,
                                const bool stabilization);
    void procGspMotorAngle( const float z, const float y);
    void procGspMotorSpeed( const float z, const float y);


signals:
    void frameSendDone();
    void passParams(void *pFrame, int size);
    void sig_checkCmd(QByteArray datagram);

    void tvFieldChanged( const float tvFieldHGrad, const float tvFieldVGrad);

    //void sig_FrameRequestTV(frame_t sFrame);
  //  void sig_FrameRequestIR(frame_t sFrame);

private:
    GST_Wrapper *m_gst_wrapper;

    QTimer* telemetry_timer;
    void telemetry_timer_send();

    void dequeuePacket();

    void enqueueTvFrame();
    void enqueueIRFrame();

    uint8_t *frame;

    uint32_t *bayer_map;
    void processPendingDatagrams();
    QUdpSocket *socket=nullptr;
    QUdpSocket* udpSocketSend = nullptr;
    Telemetry m_telemetry;
    QHostAddress dst_host;;
    quint16 dst_port;
   // void *pFrame;
   // frame_t sFrame = {0};
    int size;
    bool isBusy;


    quint64 bytesToBeWritten = 0;
    quint64 bytesWritten = 0;
    std::list<QByteArray> m_udpFifo;
    bool txActive = false;
    SettingsManager *sm;

};



#endif // NETWORK_H
