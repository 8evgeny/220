#ifndef GST_WRAPPER_H
#define GST_WRAPPER_H
#include "gst/gstelement.h"
#include "settingsmanager.h"
#include <QObject>
#include <qthread.h>
#include <qtimer.h>



class GST_Wrapper :public QObject
{

    Q_OBJECT
    QThread thread;
public:
    explicit GST_Wrapper(SettingsManager *sm = nullptr, QObject *parent = nullptr);
    ~GST_Wrapper();
signals:
    void sig_STREAM_RDY();

private:
    SettingsManager *sm;
    GstElement* run_GST_TVStream(QString videoDevice, QString clientIP, qint32 portUDP);
    GstElement* run_GST_IRStream(QString videoDevice, QString clientIP, qint32 portUDP);
    GstElement* run_GST_IRStreamISP(QString videoDevice, QString clientIP, qint32 portUDP);

    void gst_PipelineInit();
    void stopPipeline(GstElement* pipeline);
    void restartIRPipeline();
    void restartTVPipeline();
    void gst_StartIRPipeline();
    void gst_StartTVPipeline();
    void gst_Start_MPEG_TS_Pipeline();

  //  static   gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer ptr);
    void handleFpsMeasurement(uint fps, uint droprate, uint avgfps);
    int m_ir_wd = 0;
    int m_tv_wd = 0;
    GstElement* m_pipelineIR = nullptr;
    GstElement* m_pipelineTV = nullptr;
    GstElement* m_pipelineMPEG_TS = nullptr;

    GstElement* m_trackerTV = nullptr;
    GstElement* m_trackerIR = nullptr;
    bool m_IsActive_MPEG_TS = false;
    bool m_IsActiveTV = false;
    bool m_IsActiveIR = false;
    struct MpegTsParams {
        QString tvDevice;
        QString irDevice;
        QString clientIP;
        qint32  portUDP = 0;
    } m_mpegTsParams;

    GstElement  *m_mpegTsPipeline  = nullptr;
    QTimer      *m_mpegTsWatchdog  = nullptr;
    GstElement   *m_telemSrc  = nullptr;

    static constexpr qint64 WATCHDOG_TIMEOUT_MS  = 3000;  // кадры не идут N мс → рестарт
    static constexpr int    WATCHDOG_INTERVAL_MS = 15000;  // частота проверки

    std::atomic<qint64> m_tvLastFrameMs { 0 };
    std::atomic<qint64> m_irLastFrameMs { 0 };

    static GstPadProbeReturn tvFrameProbe(GstPad*, GstPadProbeInfo*, gpointer);
    static GstPadProbeReturn irFrameProbe(GstPad*, GstPadProbeInfo*, gpointer);
    GstElement* run_GST_StreamMPEG_TS(QString tvDevice, QString irDevice, QString clientIP,  qint32  portUDP);
    void restartMpegTsPipeline();
    void stopMpegTsPipeline();
    bool pushTelemetry(const void *data, gsize len, GstClockTime);

signals:
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
public slots:

    /**
     * @brief Calls the GetOutObject method on the GST Lib GOEN Tracker.
     * Retrieves the output object parameters (cx, cy, w, h).
     */
    void proc_getOutObject();

    /**
     * @brief Calls the GetOutRoi method on the GST Lib GOEN Tracker.
     * Retrieves the output ROI parameters (cx, cy, w, h).
     */
    void proc_getOutRoi();

    /**
     * @brief Calls the SetTargetObject method on the GST Lib GOEN Tracker.
     * @param cx The x-coordinate of the target object's center.
     * @param cy The y-coordinate of the target object's center.
     * @param w The width of the target object.
     * @param h The height of the target object.
     */
    void proc_setTargetObject(uint cx, uint cy, uint w, uint h);

    /**
     * @brief Calls the SetStopTr method on the GST Lib GOEN Tracker.
     */
    void proc_setStopTracking();

    /**
     * @brief Calls the GetATg method on the GST Lib GOEN Tracker.
     * Retrieves the active and locked states of the target.
     */
    void proc_getActiveLockTracking();

    /**
     * @brief Calls the SetRExt method on the GST Lib GOEN Tracker.
     * @param ext The external parameter to set (type: double).
     */
    void proc_setRoiExt(double ext);


    void proc_stopCapture();
    void onMpegTsWatchdogTimeout();
    void proc_MPEG_TS_Telemetry(QByteArray datagram);

};


static GST_Wrapper *gst_wrapper_inst = nullptr;
static int gst_pipe_cnt = 0;



#endif // GST_WRAPPER_H
