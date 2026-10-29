#ifndef GST_WRAPPER_H
#define GST_WRAPPER_H

#include <gst/gst.h>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <atomic>

#include "settingsmanager.h"
#include "track/tracker_thread.h"

class GST_Wrapper : public QObject
{
    Q_OBJECT
    friend gboolean bus_call(GstBus *, GstMessage *, gpointer);

  public:
    explicit GST_Wrapper(SettingsManager *sm = nullptr, QObject *parent = nullptr);
    ~GST_Wrapper() override;

  signals:
    void sig_STREAM_RDY();
    void sig_RstIRPipeline();
    void sig_RstTVPipeline();
    void sig_rdyOutObject(uint cx_out, uint cy_out, uint w_out, uint h_out);
    void sig_rdyOutRoi(uint cx_out, uint cy_out, uint w_out, uint h_out);
    void sig_rdyActiveLockTracking(bool active_out, bool tracking_out);

  private slots:
    void handleFpsUpdateTV(double fps, bool active_stream);
    void handleFpsUpdateIR(double fps, bool active_stream);
    void proc_RestartIRPipeline();
    void proc_RestartTVPipeline();
    void proc_Thread_Finished();
    void onMpegTsWatchdogTimeout();
    void pollBus();

  public slots:
    void proc_getOutObject();
    void proc_getOutRoi();
    void proc_setTargetObject(uint cx, uint cy, uint w, uint h);
    void proc_setStopTracking();
    void proc_getActiveLockTracking();
    void proc_setRoiExt(double ext);
    void proc_stopCapture();
    void proc_MPEG_TS_Telemetry(QByteArray datagram);
    void proc_Switch_Channel(quint32 channel, quint8 en);

  private:
    SettingsManager *sm = nullptr;
    QThread thread;

    // ─── Tracker ─────────────────────────────────────────────────
    FrameSlot m_frameSlot;
    TrackerThread *m_trackerThread = nullptr;
    std::string m_iniPath = "tracking.ini";

    // ─── Pipelines ───────────────────────────────────────────────
    GstElement *m_pipelineIR = nullptr;
    GstElement *m_pipelineTV = nullptr;
    GstElement *m_mpegTsPipeline = nullptr;
    GstElement *m_telemSrc = nullptr;

    bool m_tvEnInPipeline = false;
    bool m_irEnInPipeline = false;
    bool m_tvUseTestSrc = false;
    bool m_irUseTestSrc = false;
    bool m_IsActive_MPEG_TS = false;
    bool m_IsActiveTV = false;
    bool m_IsActiveIR = false;

    struct MpegTsParams
    {
        QString tvDevice;
        QString irDevice;
        QString clientIP;
        qint32 portUDP = 0;
    } /* struct MpegTsParams */ m_mpegTsParams;

    // ─── Bus polling (замена gst_bus_add_watch) ──────────────────
    QTimer *m_busPollTimer = nullptr;
    static constexpr int BUS_POLL_INTERVAL_MS = 50; // 20 Hz
    static constexpr int BUS_POLL_MAX_MSGS = 32;

    // ─── Watchdog (MPEG-TS) ──────────────────────────────────────
    QTimer *m_mpegTsWatchdog = nullptr;
    static constexpr qint64 WATCHDOG_TIMEOUT_MS = 5000;
    static constexpr int WATCHDOG_INTERVAL_MS = 10000;

    // ─── SRT state (latch: days without connection) ──────────────
    std::atomic<bool> m_srtSuppressWatchdog{true};
    std::atomic<qint64> m_lastSrtLogMs{0};
    static constexpr qint64 SRT_LOG_INTERVAL_MS = 60000;

    // ─── Camera watchdog timestamps ──────────────────────────────
    std::atomic<qint64> m_tvLastFrameMs{0};
    std::atomic<qint64> m_irLastFrameMs{0};

    // ─── Channels ────────────────────────────────────────────────
    static constexpr quint32 CH_TRACKER_TV = 128;
    static constexpr quint32 CH_TRACKER_IR = 129;

    enum TrackerChan
    {
        TV_CH = 0,
        IR_CH = 1
    } /* enum TrackerChan */;
    bool m_chanEnabled[2] = {false, false};
    int m_activeTrackerChannel = -1;

    // ─── Watchdog counters (non-MPEGTS) ──────────────────────────
    int m_tv_wd = 0;
    int m_ir_wd = 0;

    // ─── Pipeline builders ───────────────────────────────────────
    GstElement *run_GST_StreamMPEG_TS(QString tvDevice, QString irDevice, QString clientIP,
                                      qint32 portUDP);
    GstElement *run_GST_TVStream(QString videoDevice, QString clientIP, qint32 portUDP);
    GstElement *run_GST_IRStreamISP(QString videoDevice, QString clientIP, qint32 portUDP);

    void gst_PipelineInit();
    void stopPipeline(GstElement *pipeline);
    void restartIRPipeline();
    void restartTVPipeline();
    void gst_StartIRPipeline();
    void gst_StartTVPipeline();
    void gst_Start_MPEG_TS_Pipeline();
    void stopMpegTsPipeline();
    void restartMpegTsPipeline();
    bool pushTelemetry(const void *data, gsize len, GstClockTime);
    void enableChan(int ch);
    void disableChan(int ch);

    // ─── Pad probes ──────────────────────────────────────────────
    static GstPadProbeReturn trackerFrameProbe(GstPad *, GstPadProbeInfo *, gpointer);
    static GstPadProbeReturn tvFrameProbe(GstPad *, GstPadProbeInfo *, gpointer);
    static GstPadProbeReturn irFrameProbe(GstPad *, GstPadProbeInfo *, gpointer);
    static GstPadProbeReturn srtSinkAcceptProbe(GstPad *, GstPadProbeInfo *, gpointer); // ← ЭТО

} /* class GST_Wrapper */;

static GST_Wrapper *gst_wrapper_inst = nullptr;

#endif // GST_WRAPPER_H
