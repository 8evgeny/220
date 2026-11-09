#ifndef GST_WRAPPER_H
#define GST_WRAPPER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <atomic>
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <mutex>
#include <thread>

#include "settingsmanager.h"
#include "track/tracker_thread.h"
#include "ts_ring_buffer.h"

class GST_Wrapper : public QObject
{
    Q_OBJECT
    friend gboolean bus_call(GstBus *, GstMessage *, gpointer);

  public:
    explicit GST_Wrapper(SettingsManager *sm = nullptr, QObject *parent = nullptr);
    ~GST_Wrapper() override;

    // Graceful shutdown for process termination (SIGINT/SIGTERM), mirroring the
    // "emit Close → delete" pattern used by the other UART drivers. Stops the
    // capture/encode pipelines and tracker, then deterministically quits and joins
    // the internal worker thread so transport pipeline + pipe fds + GStreamer
    // pipelines are released before the process exits. Idempotent.
    // Returns true if the worker thread was joined; false means some thread is still
    // running — the caller MUST NOT delete this object then (destroying a running
    // QThread aborts the process with qFatal) and should terminate via _exit().
    bool shutdownGracefully(int timeoutMs = 8000);

    // Синтетический режим: ни одной реальной v4l2-камеры нет, обе веткиencode
    // подняты на videotestsrc (см. run_GST_StreamMPEG_TS). Видеотракт при этом
    // полностью рабочий. Вызывается из главного потока (watchdog Main_app),
    // пишется из worker-потока — поэтому флаг атомарный.
    bool isSyntheticVideoMode() const
    {
        return m_syntheticVideo.load(std::memory_order_relaxed);
    } // isSyntheticVideoMode

  signals:
    void sig_STREAM_RDY();
    // MPEG-TS пайп поднят с РЕАЛЬНОЙ TV-камерой (ветка не videotestsrc). Main_app
    // по нему разрешает подключение EV9500M (UART-контроллер TV-камеры). В fallback
    // (обе ветки videotestsrc) сигнал НЕ испускается — UART трогать нечего.
    void sig_RealTvCameraUp();
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
    // Вызывается ВНУТРИ worker-потока при завершении процесса: сначала остановка
    // (proc_stopCapture), затем выход из event loop → finished → proc_Thread_Finished.
    void proc_shutdownAndQuit();
    void proc_MPEG_TS_Telemetry(QByteArray datagram);
    void proc_Switch_Channel(quint32 channel, quint8 en);
    // Рестарт MPEG-TS encode-конвейера. Публичный слот: вызывается из Main_app
    // (QMetaObject::invokeMethod, QueuedConnection) для восстановления после сбоя
    // UART EV9500M. Работает в worker-потоке GST_Wrapper.
    void restartMpegTsPipeline();

  private:
    SettingsManager *sm = nullptr;
    // Указатель, а не член: если worker-поток завис и не поддаётся join'у, объект
    // QThread намеренно НЕ удаляется (удаление работающего QThread = qFatal → SIGABRT).
    QThread *thread = nullptr;

    // ─── Tracker ─────────────────────────────────────────────────
    FrameSlot m_frameSlot;
    TrackerThread *m_trackerThread = nullptr;
    std::string m_iniPath = "tracking.ini";

    // Режим завершения процесса: укороченные таймауты остановки конвейеров,
    // запрет любых рестартов и выход из event loop worker'а после очистки.
    std::atomic<bool> m_shutdownMode{false};
    // Защита от повторной остановки (proc_stopCapture приходит и по sig_GST_StopCapture,
    // и из proc_shutdownAndQuit): повторный unref конвейера = abort на выходе.
    std::atomic<bool> m_captureStopped{false};

    // ─── Pipelines ───────────────────────────────────────────────
    GstElement *m_pipelineIR = nullptr;
    GstElement *m_pipelineTV = nullptr;
    GstElement *m_mpegTsPipeline = nullptr;    // encode: cameras→mux→probe(копия в кольцо)→fakesink
    GstElement *m_transportPipeline = nullptr; // transport: appsrc←pump→srtsink/udpsink
    GstElement *m_telemSrc = nullptr;

    bool m_tvEnInPipeline = false;
    bool m_irEnInPipeline = false;
    bool m_tvUseTestSrc = false;
    bool m_irUseTestSrc = false;
    std::atomic<bool> m_syntheticVideo{false}; // обе ветки на videotestsrc (камеры недоступны)
    bool m_IsActive_MPEG_TS = false;
    bool m_IsActiveTV = false;
    bool m_IsActiveIR = false;

    struct MpegTsParams
    {
        QString tvDevice;
        QString irDevice;
        QString clientIP;
        qint32 portUDP = 0;
    } // struct MpegTsParams
    m_mpegTsParams;

    // ─── Мост encode→transport: кольцо TS + pump-поток ──────────────
    // Kernel-pipe заменён на приложение-кольцо (TsRingBuffer): producer из
    // encode (pad-probe) пишет без блокировки, отдельный pump-поток переливает
    // в appsrc транспорта. Переполнение → дроп старых целых пакетов, а НЕ
    // backpressure на encode. Это даёт независимость по backpressure.
    TsRingBuffer m_tsRing;
    std::thread m_pumpThread;
    std::atomic<bool> m_pumpRun{false}; // pump активен (encode жив)
    std::atomic<bool> m_pumpStop{false};
    std::mutex m_srcMutex;                       // защита m_pumpAppsrc от restart
    GstElement *m_pumpAppsrc = nullptr;          // текущий appsrc транспорта
    std::atomic<long long> m_tsCapturedBytes{0}; // байт, принятых в кольцо из encode (diag)
    std::atomic<long long> m_muxOutPkts{0};      // буферов на выходе mpegtsmux (diag)
    std::atomic<long long> m_muxIn256{0};        // входящие буферы на mux.sink_256 (TV h265)
    std::atomic<long long> m_muxIn257{0};        // входящие буферы на mux.sink_257 (IR h265)
    std::atomic<long long> m_muxInKlv{0};        // входящие буферы на mux.sink_512 (telem KLV)

    static constexpr std::size_t TS_RING_CAPACITY_BYTES = 256u << 10; // ~256 KiB: ограничивает задержку (drop-oldest), покрывает крупный IDR
    static constexpr std::size_t PUMP_CHUNK = 188 * 7;                // 1316 = payload SRT/RTP
    static constexpr guint64 APPSRC_MAX_BYTES = 128 * 1024;

    // ─── Bus polling (замена gst_bus_add_watch) ──────────────────
    QTimer *m_busPollTimer = nullptr;
    static constexpr int BUS_POLL_INTERVAL_MS = 50; // 20 Hz
    static constexpr int BUS_POLL_MAX_MSGS = 32;

    // ─── Watchdog (MPEG-TS) ──────────────────────────────────────
    QTimer *m_mpegTsWatchdog = nullptr;
    static constexpr qint64 WATCHDOG_TIMEOUT_MS = 5000;
    static constexpr int WATCHDOG_INTERVAL_MS = 10000;

    // ─── SRT log throttle (transport restart, no latch) ──────────
    std::atomic<qint64> m_lastSrtLogMs{0};
    static constexpr qint64 SRT_LOG_INTERVAL_MS = 60000;

    // ─── Transport restart backoff ────────────────────────────────
    int m_transportRestartCount = 0;
    static constexpr int TRANSPORT_RESTART_MAX_DELAY_MS = 30000;

    // ─── Encode start retry ───────────────────────────────────────
    // Если camera-устройство после перезагрузки ещё не готово и create pipeline
    // не удался, watchdog «немой» без pipeline → конвейер остался бы мёртвым.
    // Планируем авто-повтор запуска encode; труба/транспорт при этом остаются целыми.
    bool m_encodeStartRetryPending = false;
    static constexpr int ENCODE_START_RETRY_MS = 3000;

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
    }; // enum TrackerChan // enum TrackerChan
    bool m_chanEnabled[2] = {false, false};
    int m_activeTrackerChannel = -1;

    // ─── Watchdog counters (non-MPEGTS) ──────────────────────────
    int m_tv_wd = 0;
    int m_ir_wd = 0;

    // ─── Pipeline builders ───────────────────────────────────────
    GstElement *run_GST_StreamMPEG_TS(QString tvDevice, QString irDevice,
                                      QString clientIP, qint32 portUDP);
    GstElement *run_GST_TVStream(QString videoDevice, QString clientIP, qint32 portUDP);
    GstElement *run_GST_IRStreamISP(QString videoDevice, QString clientIP, qint32 portUDP);

    void gst_PipelineInit();
    void stopPipeline(GstElement *pipeline, gint timeout_ms = 3000);
    // Остановка/освобождение потока трекера в одной точке. quit() здесь не работает:
    // у TrackerThread собственный run()-цикл без event loop — нужен флаг + interruption.
    void stopTrackerThread();
    void restartIRPipeline();
    void restartTVPipeline();
    void gst_StartIRPipeline();
    void gst_StartTVPipeline();
    void gst_Start_MPEG_TS_Pipeline();
    void stopMpegTsPipeline();
    void serviceIspCtrl(const char *s_action);

    // ─── Мост encode→transport + transport pipeline (SRT/RTP) ──────
    void initTsBridge();                 // выделение кольца (идемпотентно)
    void deinitTsBridge();               // остановить pump + очистить кольцо
    void startPumpThread();              // запустить перелив кольцо→appsrc
    void stopPumpThread();               // join pump-потока
    void setPumpAppsrc(GstElement *src); // потокобезопасная смена appsrc
    void pumpLoop();                     // тело pump-потока
    bool startTransportPipeline();
    void stopTransportPipeline();
    void restartTransportPipeline();
    void requestKeyframes();
    static void onSrtCallerAdded(GstElement *, gint, gpointer, gpointer ud);
    bool pushTelemetry(const void *data, gsize len, GstClockTime);
    void enableChan(int ch);
    void disableChan(int ch);

    // ─── Pad probes ──────────────────────────────────────────────
    static GstPadProbeReturn trackerFrameProbe(GstPad *, GstPadProbeInfo *, gpointer);
    static GstPadProbeReturn tvFrameProbe(GstPad *, GstPadProbeInfo *, gpointer);
    static GstPadProbeReturn irFrameProbe(GstPad *, GstPadProbeInfo *, gpointer);
    // Encode→bridge: копирует каждый TS-буфер mpegtsmux в кольцо без блокировки.
    static GstPadProbeReturn mpegtsBridgeProbe(GstPad *, GstPadProbeInfo *, gpointer);
    // Диагностика: считает буферы на выходе mpegtsmux (src-pad), чтобы различать
    // «mux молчит» vs «probe на fakesink не срабатывает».
    static GstPadProbeReturn muxOutProbe(GstPad *, GstPadProbeInfo *, gpointer);
    // Считает входящие буферы на sink-pad'ах mpegtsmux (user_data = std::atomic<long long>*).
    static GstPadProbeReturn muxSinkCountProbe(GstPad *, GstPadProbeInfo *, gpointer);
    // Диагностика encode-пайплайна: состояние + caps/active каждого sink-pad mpegtsmux.
    void mpegtsDiagDump();

}; // class GST_Wrapper // class GST_Wrapper

static GST_Wrapper *gst_wrapper_inst = nullptr;

#endif // GST_WRAPPER_H
