#ifndef NETWORK_H
#define NETWORK_H

#include "gst_wrapper.h"
#include "telemetry.h"
#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>
#include <QHash>
#include <QVariant>
#include <QThread>
#include <QTimer>
#include "cmd_parser.h"
#include "settingsmanager.h"

class Network : public QObject
{
    Q_OBJECT
    QThread thread;

  public:
    explicit Network(SettingsManager *sm, GST_Wrapper *gst_wrapper, QObject *parent = nullptr);
    ~Network();
    void initSocket();
    CMD_Parser cmd_parser;

  public slots:
    void frameSend();
    void procVICAPstatus(CameraStatus status);
    void procLDCstatus(LDC_STATE dev_state);
    void procJ200status(J200_DEVSTATE ds);
    void procUpdateTrackerObject(ushort x, ushort y, ushort width, ushort height);
    void procUpdateActiveLockTracking(bool active, bool tracking);
    void proc_thread_finished();
    void proc_MINI640_State(MINI640_DEV_STATE dev_state);

    void procGspStatus(const bool connected);
    void procGspPositionMode(const bool zero, const bool pohod, const bool pilot, const bool park,
                             const bool stabilization);
    void procGspMotorAngle(const float z, const float y);
    void procGspMotorSpeed(const float z, const float y);
    void procSetTrackerChannel(quint8 ch_id);

  signals:
    void frameSendDone();
    void passParams(void *pFrame, int size);
    void sig_checkCmd(QByteArray datagram);
    void tvFieldChanged(const float tvFieldHGrad, const float tvFieldVGrad);
    void sig_irFieldChanged(const float fovW, const float fovH);
    void sig_MPEG_TS_Telemetry(QByteArray datagram);

  private slots:
    // ─── UDP ─────────────────────────────────────────────────────
    void processPendingDatagrams();

    // ─── TCP ─────────────────────────────────────────────────────
    void onTcpNewConnection();
    void onTcpReadyRead();
    void onTcpDisconnected();

  private:
    void telemetry_timer_send();
    void dequeuePacket();

    // ─── Common cmd dispatch ─────────────────────────────────────
    void handleIncomingCommand(const QByteArray &packet, const QString &source);

    GST_Wrapper *m_gst_wrapper = nullptr;
    SettingsManager *sm = nullptr;

    // ─── UDP ─────────────────────────────────────────────────────
    QUdpSocket *socket = nullptr;
    QUdpSocket *udpSocketSend = nullptr;
    QHostAddress dst_host;
    quint16 dst_port = 0;

    // ─── TCP Server ──────────────────────────────────────────────
    QTcpServer *m_tcpServer = nullptr;
    QHash<QTcpSocket *, QByteArray> m_tcpBuffers;

    // ─── Telemetry ───────────────────────────────────────────────
    QTimer *telemetry_timer = nullptr;
    Telemetry m_telemetry;
    QByteArray m_TelemPreamb;

    // ─── UDP TX queue (non-MPEGTS mode) ──────────────────────────
    std::list<QByteArray> m_udpFifo;
    bool txActive = false;
    quint64 bytesToBeWritten = 0;
    quint64 bytesWritten = 0;

    // ─── Constants ───────────────────────────────────────────────
    static constexpr int CMD_PACKET_SIZE = 516;
    static constexpr int TCP_MAX_BUFFER = CMD_PACKET_SIZE * 20; // overflow guard
    static constexpr int UDP_TX_BATCH = 20;
} /* class Network */;

#endif // NETWORK_H
