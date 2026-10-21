#include "gst_wrapper.h"

#include <gst/gst.h>
#include <gst/video/video.h>
#include <QDateTime>
#include <QProcess>
#include <QTimer>
#include <QCoreApplication>

// ═══════════════════════════════════════════════════════════════════
//  BUS CALL — called from pollBus() (NOT GSource/GMainLoop)
// ═══════════════════════════════════════════════════════════════════
gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer ptr)
{
    Q_UNUSED(bus);
    auto *self = static_cast<GST_Wrapper*>(ptr);

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
        break;

    case GST_MESSAGE_ERROR: {
        gchar *debug = nullptr;
        GError *error = nullptr;
        gst_message_parse_error(msg, &error, &debug);

        const gchar *srcName = GST_MESSAGE_SRC(msg)
                                   ? GST_OBJECT_NAME(GST_MESSAGE_SRC(msg)) : "";

        if (g_str_has_prefix(srcName, "mpeg_ts_srt") && self) {
            // SRT error → suppress watchdog, pipeline continues
            self->m_srtSuppressWatchdog.store(true);

            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (now - self->m_lastSrtLogMs.load() > GST_Wrapper::SRT_LOG_INTERVAL_MS) {
                g_printerr("[SRT] ERROR: %s — suppressed\n",
                           error ? error->message : "?");
                self->m_lastSrtLogMs.store(now);
            }
        } else {
            // Critical error (camera/encoder) → restart
            g_printerr("FATAL ERROR: %s (%s)\n",
                       error ? error->message : "?", srcName);
            if (self && self->m_mpegTsPipeline) {
                QMetaObject::invokeMethod(self, [self]() {
                    self->restartMpegTsPipeline();
                }, Qt::QueuedConnection);
            }
        }

        if (debug) g_free(debug);
        g_error_free(error);
        break;
    }

    case GST_MESSAGE_WARNING: {
        GError *warning = nullptr;
        gchar  *debug   = nullptr;
        gst_message_parse_warning(msg, &warning, &debug);

        const gchar *srcName = GST_MESSAGE_SRC(msg)
                                   ? GST_OBJECT_NAME(GST_MESSAGE_SRC(msg)) : "";
        const QString desc = warning ? QString::fromUtf8(warning->message) : QString();

        const bool isSrtIssue = g_str_has_prefix(srcName, "mpeg_ts_srt") ||
                                desc.contains("Connection timeout", Qt::CaseInsensitive);

        if (isSrtIssue && self) {
            self->m_srtSuppressWatchdog.store(true);

            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (now - self->m_lastSrtLogMs.load() > GST_Wrapper::SRT_LOG_INTERVAL_MS) {
                g_printerr("[SRT] WARNING: %s — suppressed\n", desc.toUtf8().constData());
                self->m_lastSrtLogMs.store(now);
            }
        } else {
            g_printerr("[GST WARN] %s: %s\n", srcName, warning ? warning->message : "?");
        }

        if (warning) g_error_free(warning);
        g_free(debug);
        break;
    }

    default:
        break;
    }
    return TRUE;
}

// ═══════════════════════════════════════════════════════════════════
//  BUS POLLING — drains bus via QTimer (Qt event loop compatible)
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::pollBus()
{
    if (!m_mpegTsPipeline) return;

    GstBus *bus = gst_element_get_bus(m_mpegTsPipeline);
    if (!bus) return;

    int count = 0;
    while (count < BUS_POLL_MAX_MSGS) {
        GstMessage *msg = gst_bus_pop_filtered(bus, static_cast<GstMessageType>(
                                                        GST_MESSAGE_ERROR | GST_MESSAGE_WARNING | GST_MESSAGE_EOS));
        if (!msg) break;

        bus_call(bus, msg, this);
        gst_message_unref(msg);
        count++;
    }

    gst_object_unref(bus);
}

// ═══════════════════════════════════════════════════════════════════
//  CONSTRUCTOR / DESTRUCTOR
// ═══════════════════════════════════════════════════════════════════
GST_Wrapper::GST_Wrapper(SettingsManager *sm, QObject *parent)
    : QObject(parent), sm{sm}
{
    qDebug() << Q_FUNC_INFO;

    connect(&thread, &QThread::started, this, [this] {
        gst_PipelineInit();
        qDebug() << "GST Pipe init done";
    }, Qt::DirectConnection);

    QObject::connect(&thread, &QThread::finished, this, &GST_Wrapper::proc_Thread_Finished);

    moveToThread(&thread);
    thread.start();
    gst_wrapper_inst = this;
}

GST_Wrapper::~GST_Wrapper()
{
    m_IsActive_MPEG_TS = false;
    m_IsActiveTV = false;
    m_IsActiveIR = false;

    if (!thread.wait(5000)) {
        thread.terminate();
        thread.wait(1000);
    }
}

// ═══════════════════════════════════════════════════════════════════
//  PAD PROBES
// ═══════════════════════════════════════════════════════════════════
GstPadProbeReturn GST_Wrapper::trackerFrameProbe(
    GstPad *pad, GstPadProbeInfo *info, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper*>(userData);

    if (!(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_BUFFER))
        return GST_PAD_PROBE_OK;

    GstBuffer *buf = GST_PAD_PROBE_INFO_BUFFER(info);
    if (!buf) return GST_PAD_PROBE_OK;

    GstElement *parent = gst_pad_get_parent_element(pad);
    int ch = -1;
    if (parent) {
        const gchar *name = GST_OBJECT_NAME(parent);
        if (g_str_has_prefix(name, "tv_"))      ch = 0;
        else if (g_str_has_prefix(name, "ir_")) ch = 1;
        gst_object_unref(parent);
    }

    if (ch != self->m_activeTrackerChannel)
        return GST_PAD_PROBE_OK;

    GstCaps *caps = gst_pad_get_current_caps(pad);
    if (!caps) return GST_PAD_PROBE_OK;

    GstVideoInfo vinfo;
    if (!gst_video_info_from_caps(&vinfo, caps)) {
        gst_caps_unref(caps);
        return GST_PAD_PROBE_OK;
    }
    gst_caps_unref(caps);

    GstMapInfo map;
    if (!gst_buffer_map(buf, &map, GST_MAP_READ))
        return GST_PAD_PROBE_OK;

    cv::Mat frame(static_cast<int>(vinfo.height),
                  static_cast<int>(vinfo.width), CV_8UC3, map.data);
    self->m_frameSlot.write(frame, ch);

    gst_buffer_unmap(buf, &map);
    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn GST_Wrapper::tvFrameProbe(GstPad*, GstPadProbeInfo*, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper*>(userData);
    self->m_tvLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn GST_Wrapper::irFrameProbe(GstPad*, GstPadProbeInfo*, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper*>(userData);
    self->m_irLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    return GST_PAD_PROBE_OK;
}
// ═══════════════════════════════════════════════════════════════════
//  SRT SINK ACCEPT PROBE — fires ONLY when data reaches srtsink.
//  With wait-for-connection=true + no listener → never fires.
//  When listener appears → mux unblocks → data flows → probe fires
//  → suppress=false → watchdog starts checking cameras again.
// ═══════════════════════════════════════════════════════════════════

GstPadProbeReturn GST_Wrapper::srtSinkAcceptProbe(GstPad*, GstPadProbeInfo* info, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper*>(userData);

    if (!(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_BUFFER))
        return GST_PAD_PROBE_OK;

    // Data actually flowing into srtsink → SRT connection is alive!
    if (self->m_srtSuppressWatchdog.load()) {
        self->m_srtSuppressWatchdog.store(false);

        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - self->m_lastSrtLogMs.load() > SRT_LOG_INTERVAL_MS) {
            g_printerr("[SRT] Connected — watchdog re-enabled\n");
            self->m_lastSrtLogMs.store(now);
        }
    }

    return GST_PAD_PROBE_OK;
}

// ═══════════════════════════════════════════════════════════════════
//  Tracker / Channel management
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::enableChan(int ch)
{
    if (ch < 0 || ch > 1 || !m_trackerThread) return;

    const bool wasActive = m_activeTrackerChannel >= 0 && m_activeTrackerChannel != ch;

    // ═══ ШАГ 1: Немедленно обнулить результаты ═══════════════════
    // Теперь proc_getActiveLockTracking() → active=false, locked=false
    // GyroPlatformHandler прекратит управление платформой СРАЗУ.
    m_trackerThread->forceResetResults();

    // ═══ ШАГ 2: Переключить канал приёма кадров ══════════════════
    m_chanEnabled[1 - ch] = false;
    m_chanEnabled[ch]     = true;
    m_activeTrackerChannel = ch;
    m_trackerThread->setActiveChannel(ch);

    // ═══ ШАГ 3: Остановить и пересоздать трекер в потоке ═════════
    TrackerCmd cmd;
    if (wasActive) {
        cmd.type    = TrackerCmd::SWITCH;   // stopTracking + reset + ini
        cmd.iniPath = m_iniPath;
    } else {
        cmd.type    = TrackerCmd::REINIT;
        cmd.iniPath = m_iniPath;
    }
    m_trackerThread->submitCmd(cmd);

    qInfo() << "[Tracker]" << (ch == TV_CH ? "TV" : "IR")
            << "enabled" << (wasActive ? "(switched)" : "(initial)");
}


void GST_Wrapper::disableChan(int ch)
{
    if (ch < 0 || ch > 1) return;
    m_chanEnabled[ch] = false;

    if (m_activeTrackerChannel == ch && m_trackerThread) {
        // Немедленно — результаты невалидны
        m_trackerThread->forceResetResults();

        // Потом — остановить трекер в его потоке
        TrackerCmd cmd;
        cmd.type = TrackerCmd::STOP;
        m_trackerThread->submitCmd(cmd);
        m_trackerThread->setActiveChannel(-1);
        m_activeTrackerChannel = -1;
    }
}


void GST_Wrapper::proc_Switch_Channel(quint32 channel, quint8 en)
{
    int ch = (channel == CH_TRACKER_TV) ? TV_CH :
                 (channel == CH_TRACKER_IR) ? IR_CH : -1;
    if (ch < 0) return;

    if (en) {
        enableChan(ch);
    } else {
        disableChan(ch);
    }
}

void GST_Wrapper::proc_getOutObject()
{
    if (!m_trackerThread) return;
    auto r = m_trackerThread->objectRect();
    emit sig_rdyOutObject(r.cx, r.cy, r.w, r.h);
}

void GST_Wrapper::proc_getOutRoi()
{
    if (!m_trackerThread) return;
    auto r = m_trackerThread->roiRect();
    emit sig_rdyOutRoi(r.cx, r.cy, r.w, r.h);
}

void GST_Wrapper::proc_getActiveLockTracking()
{
    if (!m_trackerThread) return;
    emit sig_rdyActiveLockTracking(m_trackerThread->isActive(), m_trackerThread->isLockedOn());
}

void GST_Wrapper::proc_setTargetObject(uint cx, uint cy, uint w, uint h)
{
    if (!m_trackerThread) return;
    TrShWrapper::Rect rect;
    rect.cx = static_cast<unsigned short>(cx - (w / 2));
    rect.cy = static_cast<unsigned short>(cy - (h / 2));
    rect.w  = static_cast<unsigned short>(w);
    rect.h  = static_cast<unsigned short>(h);

    TrackerCmd cmd;
    cmd.type   = TrackerCmd::SET_TARGET;
    cmd.target = rect;
    m_trackerThread->submitCmd(cmd);
}

void GST_Wrapper::proc_setStopTracking()
{
    if (!m_trackerThread) return;
    TrackerCmd cmd;
    cmd.type = TrackerCmd::STOP;
    m_trackerThread->submitCmd(cmd);
}

void GST_Wrapper::proc_setRoiExt(double ext) { Q_UNUSED(ext); }

// ═══════════════════════════════════════════════════════════════════
//  FPS handlers (non-MPEGTS mode)
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::handleFpsUpdateTV(double fps, bool active_stream)
{
    if (fps < 0.1) {
        if (m_tv_wd < 10) {
            m_tv_wd++;
            qDebug() << "TV stream stalled. WD=" << m_tv_wd;
        } else {
            qDebug() << "TV stream cannot resume. Restart...";
            emit sig_RstTVPipeline();
            m_tv_wd = 0;
        }
    } else {
        qDebug() << "TV FPS:" << fps << "active:" << active_stream;
        m_tv_wd = 0;
    }
}

void GST_Wrapper::handleFpsUpdateIR(double fps, bool active_stream)
{
    if (fps < 0.1) {
        if (m_ir_wd < 10) {
            m_ir_wd++;
            qDebug() << "IR stream stalled. WD=" << m_ir_wd;
        } else {
            qDebug() << "IR stream cannot resume. Restart...";
            emit sig_RstIRPipeline();
            m_ir_wd = 0;
        }
    } else {
        qDebug() << "IR FPS:" << fps << "active:" << active_stream;
        m_ir_wd = 0;
    }
}
// ═══════════════════════════════════════════════════════════════════
//  CAMERA CHECK HELPERS
// ═══════════════════════════════════════════════════════════════════
static bool v4l2CtlHasFrame(const QString& device, int width, int height,
                            const QString& pixelformat, int timeoutMs)
{
    QProcess process;
    QStringList args;
    args << "-d" << device;
    if (width > 0 && height > 0)
        args << "--set-fmt-video=" + QString("width=%1,height=%2,pixelformat=%3")
                                         .arg(width).arg(height).arg(pixelformat);
    args << "--stream-mmap=3" << "--stream-count=1" << "--stream-to=/dev/null";

    process.start("v4l2-ctl", args);
    if (!process.waitForStarted(1000)) return false;
    if (!process.waitForFinished(timeoutMs)) { process.kill(); process.waitForFinished(); return false; }
    return (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0);
}

static bool v4l2CtlCheckWithRetries(const QString& name, const QString& device,
                                    int w, int h, const QString& fmt,
                                    int firstTo, int retryTo, int retries)
{
    for (int i = 0; i <= retries; ++i) {
        g_printerr("[CAM CHECK] %s attempt %d/%d\n", name.toUtf8().constData(), i+1, retries+1);
        if (v4l2CtlHasFrame(device, w, h, fmt, i == 0 ? firstTo : retryTo)) return true;
    }
    g_printerr("[CAM CHECK] %s FAILED\n", name.toUtf8().constData());
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//  WATCHDOG (MPEG-TS)
//
//  Если SRT не подключён (suppress=true):
//    → проверяем только что pipeline PLAYING. Не трогаем ничего else.
//
//  Если SRT подключён (suppress=false):
//    → полный check: state + camera staleness
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::onMpegTsWatchdogTimeout()
{
    if (!m_mpegTsPipeline) return;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    GstState current = GST_STATE_VOID_PENDING;
    gst_element_get_state(m_mpegTsPipeline, &current, nullptr, 0);

    // SRT suppressed → only check pipeline alive
    if (m_srtSuppressWatchdog.load()) {
        if (current != GST_STATE_PLAYING) {
            qWarning() << "[Watchdog] Pipeline NOT PLAYING — restart";
            m_srtSuppressWatchdog.store(false);
            restartMpegTsPipeline();
        }
        return;  // ← КАМЕРЫ МОГУТ БЫТЬ "STALE" ИЗ-ЗА BACK-PRESSURE — НЕ ПЕРЕЗАПУСКАЕМ!
    }

    // SRT connected → full check
    if (current != GST_STATE_PLAYING) {
        qWarning() << "[Watchdog] Pipeline not PLAYING — restart";
        restartMpegTsPipeline();
        return;
    }

    const bool tvStale = m_tvEnInPipeline && !m_tvUseTestSrc &&
                         (now - m_tvLastFrameMs.load() > WATCHDOG_TIMEOUT_MS);
    const bool irStale = m_irEnInPipeline && !m_irUseTestSrc &&
                         (now - m_irLastFrameMs.load() > WATCHDOG_TIMEOUT_MS);

    if (tvStale || irStale) {
        qWarning() << "[Watchdog] Camera stale tv=" << tvStale << "ir=" << irStale;
        restartMpegTsPipeline();
    }
}

// ═══════════════════════════════════════════════════════════════════
//  FORCE STOP PIPELINE HELPER
// ═══════════════════════════════════════════════════════════════════
static bool forceStopPipeline(GstElement *pipeline, gint timeout_ms = 3000)
{
    if (!pipeline) return true;

    gst_element_send_event(pipeline, gst_event_new_eos());
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_NULL);
    if (ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_NO_PREROLL)
        return true;

    GstState current = GST_STATE_VOID_PENDING;
    ret = gst_element_get_state(pipeline, &current, nullptr, timeout_ms * GST_MSECOND);
    if (ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_NO_PREROLL)
        return true;

    qWarning() << "Pipeline stuck in" << gst_element_state_get_name(current)
               << ", sending FLUSH";

    GstIterator *pads = gst_element_iterate_pads(pipeline);
    GValue item = G_VALUE_INIT;
    gboolean done = FALSE;
    while (!done) {
        switch (gst_iterator_next(pads, &item)) {
        case GST_ITERATOR_OK: {
            GstPad *pad = GST_PAD(g_value_get_object(&item));
            gst_pad_send_event(pad, gst_event_new_flush_start());
            gst_pad_send_event(pad, gst_event_new_flush_stop(TRUE));
            g_value_unset(&item);
            break;
        }
        case GST_ITERATOR_DONE: done = TRUE; break;
        case GST_ITERATOR_RESYNC: gst_iterator_resync(pads); break;
        case GST_ITERATOR_ERROR: done = TRUE; break;
        }
    }
    g_value_unset(&item);
    gst_iterator_free(pads);

    ret = gst_element_get_state(pipeline, &current, nullptr, timeout_ms * GST_MSECOND);
    return (ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_NO_PREROLL);
}

// ═══════════════════════════════════════════════════════════════════
//  PIPELINE INIT / START / STOP / RESTART
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::gst_PipelineInit()
{
    emit sig_STREAM_RDY();

    if (sm->settings_store.mpeg_ts_en) {
        gst_Start_MPEG_TS_Pipeline();
    } else {
        connect(this, &GST_Wrapper::sig_RstIRPipeline, this, &GST_Wrapper::proc_RestartIRPipeline);
        connect(this, &GST_Wrapper::sig_RstTVPipeline, this, &GST_Wrapper::proc_RestartTVPipeline);
        gst_StartTVPipeline();
        gst_StartIRPipeline();
    }
    emit sig_STREAM_RDY();
}

void GST_Wrapper::gst_Start_MPEG_TS_Pipeline()
{
    m_mpegTsPipeline = run_GST_StreamMPEG_TS(
        sm->settings_store.tv_cam_video_device,
        sm->settings_store.ir_cam_isp_video_device,
        sm->settings_store.ip_host,
        sm->settings_store.mpeg_ts_port);

    if (!m_mpegTsPipeline) { g_printerr("Failed to create MPEG-TS pipeline\n"); return; }

    // Bus polling (NO gst_bus_add_watch — requires GLib loop which is absent)
    if (!m_busPollTimer) {
        m_busPollTimer = new QTimer(this);
        m_busPollTimer->setInterval(BUS_POLL_INTERVAL_MS);
        connect(m_busPollTimer, &QTimer::timeout, this, &GST_Wrapper::pollBus);
    }
    m_busPollTimer->start();

    gst_element_set_state(m_mpegTsPipeline, GST_STATE_PLAYING);
    m_IsActive_MPEG_TS = true;
}

void GST_Wrapper::gst_StartTVPipeline()
{
    m_pipelineTV = run_GST_TVStream(sm->settings_store.tv_cam_video_device,
                                    sm->settings_store.ip_host, 5000);
    if (!m_pipelineTV) { g_printerr("Failed to create TV pipeline\n"); return; }
    gst_element_set_state(m_pipelineTV, GST_STATE_PLAYING);
    m_IsActiveTV = true;
}

void GST_Wrapper::gst_StartIRPipeline()
{
    m_pipelineIR = run_GST_IRStreamISP(sm->settings_store.ir_cam_isp_video_device,
                                       sm->settings_store.ip_host, 5008);
    if (!m_pipelineIR) { g_printerr("Failed to create IR pipeline\n"); return; }
    gst_element_set_state(m_pipelineIR, GST_STATE_PLAYING);
    m_IsActiveIR = true;
}

void GST_Wrapper::stopPipeline(GstElement* pipeline)
{
    if (pipeline) { gst_element_set_state(pipeline, GST_STATE_NULL); gst_object_unref(pipeline); }
}

void GST_Wrapper::restartIRPipeline()
{
    if (m_pipelineIR) { stopPipeline(m_pipelineIR); m_IsActiveIR = false; }
    QTimer::singleShot(100, this, &GST_Wrapper::gst_StartIRPipeline);
}

void GST_Wrapper::restartTVPipeline()
{
    if (m_pipelineTV) stopPipeline(m_pipelineTV);
    QTimer::singleShot(100, this, &GST_Wrapper::gst_StartTVPipeline);
}

void GST_Wrapper::proc_RestartIRPipeline() { restartIRPipeline(); }
void GST_Wrapper::proc_RestartTVPipeline() { restartTVPipeline(); }

void GST_Wrapper::stopMpegTsPipeline()
{
    m_IsActive_MPEG_TS = false;
    if (m_busPollTimer) m_busPollTimer->stop();
    if (m_mpegTsWatchdog) m_mpegTsWatchdog->stop();

    if (m_trackerThread) {
        TrackerCmd cmd; cmd.type = TrackerCmd::STOP;
        m_trackerThread->submitCmd(cmd);
        m_trackerThread->setActiveChannel(-1);
    }

    m_telemSrc = nullptr;
    m_tvEnInPipeline = false;  m_irEnInPipeline = false;
    m_tvUseTestSrc   = false;  m_irUseTestSrc   = false;
    m_activeTrackerChannel = -1;
    m_chanEnabled[TV_CH] = false; m_chanEnabled[IR_CH] = false;

    if (m_mpegTsPipeline) {
        forceStopPipeline(m_mpegTsPipeline, 3000);
        gst_object_unref(m_mpegTsPipeline);
        m_mpegTsPipeline = nullptr;
    }
}

void GST_Wrapper::restartMpegTsPipeline()
{
    if (!m_IsActive_MPEG_TS) return;
    stopMpegTsPipeline();
    QTimer::singleShot(5000, this, [this]() {
        if (m_IsActive_MPEG_TS) gst_Start_MPEG_TS_Pipeline();
    });
}

void GST_Wrapper::proc_stopCapture()
{
    if (m_pipelineIR) { stopPipeline(m_pipelineIR); m_IsActiveIR = false; }
    if (m_pipelineTV) { stopPipeline(m_pipelineTV); m_IsActiveTV = false; }
    if (m_mpegTsPipeline) { m_IsActive_MPEG_TS = false; stopMpegTsPipeline(); }

    if (m_trackerThread) {
        m_trackerThread->quit();
        m_trackerThread->wait(3000);
        delete m_trackerThread;
        m_trackerThread = nullptr;
    }
}

void GST_Wrapper::proc_Thread_Finished()
{
    if (m_busPollTimer) { m_busPollTimer->stop(); delete m_busPollTimer; m_busPollTimer = nullptr; }

    if (m_trackerThread) {
        m_trackerThread->quit(); m_trackerThread->wait(3000);
        delete m_trackerThread; m_trackerThread = nullptr;
    }
    if (m_pipelineIR)   { gst_element_set_state(m_pipelineIR, GST_STATE_NULL);   gst_object_unref(m_pipelineIR);   m_pipelineIR = nullptr; }
    if (m_pipelineTV)   { gst_element_set_state(m_pipelineTV, GST_STATE_NULL);   gst_object_unref(m_pipelineTV);   m_pipelineTV = nullptr; }
    if (m_mpegTsPipeline){ gst_element_set_state(m_mpegTsPipeline, GST_STATE_NULL); gst_object_unref(m_mpegTsPipeline); m_mpegTsPipeline = nullptr; }

    if (m_mpegTsWatchdog) { m_mpegTsWatchdog->stop(); delete m_mpegTsWatchdog; m_mpegTsWatchdog = nullptr; }
}

// ═══════════════════════════════════════════════════════════════════
//  run_GST_StreamMPEG_TS — MAIN PIPELINE BUILDER
//
//  KEY DESIGN FOR AUTONOMOUS OPERATION (days without SRT):
//
//  tv_enc_queue: leaky=2, max-buffers=4   ← drops frames when mux blocked
//  ir_enc_queue: leaky=2, max-buffers=4   ← drops frames when mux blocked
//  mux_output_queue: NOT leaky, max-buffers=4  ← small buffer before srtsink
//
//  When SRT is not connected (wait-for-connection=true):
//    srtsink does NOT accept data → mux blocks on src pad
//    tv_enc_queue / ir_enc_queue are LEAKY → drop frames, encoder NOT blocked
//    tee NOT blocked → tracker branch WORKS
//
//  When SRT connects:
//    srtsink starts accepting → mux unblocks → TS flows normally
// ═══════════════════════════════════════════════════════════════════
GstElement* GST_Wrapper::run_GST_StreamMPEG_TS(
    QString tvDevice, QString irDevice, QString clientIP, qint32 portUDP)
{
    m_mpegTsParams = { tvDevice, irDevice, clientIP, portUDP };

    const QByteArray tvBA = tvDevice.toLocal8Bit();
    const QByteArray irBA = irDevice.toLocal8Bit();
    const QByteArray ipBA = clientIP.toLocal8Bit();
    const auto& cfg = sm->settings_store;

    bool tvEn  = cfg.tv_cam_en;
    bool irEn  = cfg.ir_cam_en;
    const bool telEn = cfg.mpeg_ts_telemetry_en;

    m_tvUseTestSrc = false;
    m_irUseTestSrc = false;

    if (tvEn) {
        tvEn = v4l2CtlCheckWithRetries("TV", cfg.tv_cam_video_device,
                                       cfg.tv_cam_width, cfg.tv_cam_height, "RGB3",
                                       cfg.tv_cam_first_frame_timeout_ms,
                                       cfg.cam_frame_retry_timeout_ms,
                                       cfg.cam_frame_restart_count);
    }
    if (irEn) {
        irEn = v4l2CtlCheckWithRetries("IR", cfg.ir_cam_isp_video_device,
                                       cfg.ir_cam_width, cfg.ir_cam_height, "NV12",
                                       cfg.ir_cam_first_frame_timeout_ms,
                                       cfg.cam_frame_retry_timeout_ms,
                                       cfg.cam_frame_restart_count);
    }
    if (!tvEn && !irEn) {
        g_printerr("[MPEG-TS] No cameras → videotestsrc fallback\n");
        tvEn = true; irEn = true;
        m_tvUseTestSrc = true; m_irUseTestSrc = true;
    }

    m_tvEnInPipeline = tvEn;
    m_irEnInPipeline = irEn;

    if (!tvEn && !irEn && !telEn) { g_printerr("[MPEG-TS] Nothing enabled\n"); return nullptr; }

    const bool tvFlip = cfg.tv_cam_flip_180;
    const bool irFlip = cfg.ir_cam_flip_180;
    const bool useSrt = cfg.mpeg_ts_use_srt;

    const int srtLatencyMs   = qBound(20, cfg.mpeg_ts_srt_latency_ms, 5000);
    const bool srtTlpktdrop  = cfg.mpeg_ts_srt_tlpktdrop;
    const int srtTsAlignment = cfg.mpeg_ts_srt_ts_packet_count;
    const int srtPayloadSize = srtTsAlignment * 188;

    const bool fecEnabled = cfg.mpeg_ts_srt_fec_enabled;
    const int  fecCols    = qBound(2, cfg.mpeg_ts_srt_fec_cols, 255);
    const int  fecRows    = qBound(1, cfg.mpeg_ts_srt_fec_rows, 255);

    QString fecLayout = cfg.mpeg_ts_srt_fec_layout.trimmed().toLower();
    if (fecLayout != "staircase" && fecLayout != "even") fecLayout = "staircase";
    QString fecArq = cfg.mpeg_ts_srt_fec_arq.trimmed().toLower();
    if (fecArq != "always" && fecArq != "onreq" && fecArq != "never") fecArq = "always";

    QString srtUri;
    if (useSrt) {
        srtUri = QString("srt://%1:%2?mode=caller&latency=%3&payloadsize=%4&tlpktdrop=%5")
        .arg(clientIP).arg(portUDP).arg(srtLatencyMs)
            .arg(srtPayloadSize).arg(srtTlpktdrop ? "true" : "false");
        if (fecEnabled)
            srtUri += QString("&packetfilter=fec,cols:%1,rows:%2,layout:%3,arq:%4")
                          .arg(fecCols).arg(fecRows).arg(fecLayout).arg(fecArq);
    }

    const int muxAlignment = useSrt ? srtTsAlignment : 7;

    // ─── Build pipeline string ─────────────────────────────────────
    QString pipe;
    auto appendPipe = [&](const QString &s) { if (!pipe.isEmpty()) pipe += " "; pipe += s; };

    // TV branch
    if (tvEn) {
        QString tvSource = m_tvUseTestSrc
                               ? QString("videotestsrc name=tv_testsrc is-live=true do-timestamp=true pattern=0 ! "
                                         "video/x-raw,format=BGR,width=%1,height=%2,framerate=%3/1")
                                     .arg(cfg.tv_cam_width).arg(cfg.tv_cam_height).arg(cfg.tv_cam_target_fps)
                               : QString("v4l2src device=%1 do-timestamp=true ! "
                                         "video/x-raw,format=BGR,width=%2,height=%3")
                                     .arg(tvBA.data()).arg(cfg.tv_cam_width).arg(cfg.tv_cam_height);

        appendPipe(tvSource + QString(
                                  " ! queue leaky=2 max-size-buffers=2 max-size-bytes=0 max-size-time=0 ! tee name=tv_tee"
                                  // Encoder branch → mux.sink_256
                                  " tv_tee. ! queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !"  // ← LEAKY! protects tee
                                  " videoconvert ! video/x-raw,format=NV12 !"
                                  " videoflip name=tv_flip method=%1 !"
                                  " videorate ! video/x-raw,framerate=%2/1 !"
                                  " mpph265enc name=tv_enc ! h265parse config-interval=2 !"
                                  " video/x-h265,stream-format=byte-stream,alignment=au !"
                                  " queue name=tv_enc_queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !"  // ← LEAKY! protects tee
                                  " mux.sink_256"
                                  // Tracker branch → fakesink
                                  " tv_tee. ! queue leaky=2 max-size-buffers=1 max-size-bytes=0 max-size-time=0 !"
                                  " videoconvert ! video/x-raw,format=RGB,width=%3,height=%4 !"
                                  " videoflip name=tv_flip_track method=%1 !"
                                  " fakesink name=tv_trk_sink sync=false")
                                  .arg(tvFlip ? "rotate-180" : "none")
                                  .arg(cfg.tv_cam_target_fps)
                                  .arg(cfg.tv_cam_width).arg(cfg.tv_cam_height));
    }

    // IR branch
    if (irEn) {
        QString irSource = m_irUseTestSrc
                               ? QString("videotestsrc name=ir_testsrc is-live=true do-timestamp=true pattern=0 ! "
                                         "video/x-raw,format=I420,width=%1,height=%2,framerate=%3/1")
                                     .arg(cfg.ir_cam_width).arg(cfg.ir_cam_height).arg(cfg.ir_cam_target_fps)
                               : QString("v4l2src device=%1 do-timestamp=true ! "
                                         "video/x-raw,width=%2,height=%3,framerate=%4/1")
                                     .arg(irBA.data()).arg(cfg.ir_cam_width).arg(cfg.ir_cam_height)
                                     .arg(cfg.ir_cam_target_fps);

        appendPipe(irSource + QString(
                                  " ! queue leaky=2 max-size-buffers=2 max-size-bytes=0 max-size-time=0 !"
                                  " videorate ! video/x-raw,framerate=%1/1 ! tee name=ir_tee"
                                  // Encoder branch → mux.sink_257
                                  " ir_tee. ! queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !"  // ← LEAKY! protects tee
                                  " videoflip name=ir_flip method=%2 !"
                                  " mpph265enc name=ir_enc ! h265parse config-interval=2 !"
                                  " video/x-h265,stream-format=byte-stream,alignment=au !"
                                  " queue name=ir_enc_queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !"  // ← LEAKY! protects tee
                                  " mux.sink_257"
                                  // Tracker branch → fakesink
                                  " ir_tee. ! queue leaky=2 max-size-buffers=1 max-size-bytes=0 max-size-time=0 !"
                                  " videoconvert ! video/x-raw,format=RGB,width=%3,height=%4 !"
                                  " videoflip name=ir_flip_track method=%2 !"
                                  " fakesink name=ir_trk_sink sync=false")
                                  .arg(cfg.ir_cam_target_fps)
                                  .arg(irFlip ? "rotate-180" : "none")
                                  .arg(cfg.ir_cam_width).arg(cfg.ir_cam_height));
    }

    // Telemetry
    if (telEn) {
        appendPipe(
            "appsrc name=telem_src is-live=true format=time do-timestamp=true "
            "caps=\"meta/x-klv,parsed=true\" !"
            " queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !"  // ← LEAKY! protects appsrc
            " mux.sink_512");
    }

    // Mux + output: SMALL non-leaky queue (just for SRT packet alignment)
    appendPipe(QString(
                   "mpegtsmux name=mux alignment=%1 pcr-interval=40000 pat-interval=40000"
                   " pmt-interval=40000 !"
                   " queue name=mux_output_queue max-size-buffers=7 max-size-bytes=0 max-size-time=0 !")
                   .arg(muxAlignment));

    if (useSrt) {
        // wait-for-connection=true: srtsink does NOT accept data until listener appears.
        // This means mux blocks on src pad → leaky queues before mux drop frames.
        // Cameras and tracker continue working through tee.
        appendPipe(QString(
                       "srtsink name=mpeg_ts_srt_sink uri=\"%1\" latency=%2"
                       " auto-reconnect=true wait-for-connection=true sync=false async=false")
                       .arg(srtUri).arg(srtLatencyMs));
    } else {
        appendPipe(QString("rtpmp2tpay pt=33 mtu=1328 ! udpsink host=%1 port=%2"
                           " sync=false async=false").arg(ipBA.data()).arg(portUDP));
    }

    g_printerr("\n============================================================\n");
    g_printerr("MPEG-TS: %s | TV:%s%s IR:%s%s Tel:%s\n",
               useSrt ? "SRT" : "RTP/UDP",
               tvEn ? "Y" : "N", m_tvUseTestSrc ? "(test)" : "",
               irEn ? "Y" : "N", m_irUseTestSrc ? "(test)" : "",
               telEn ? "Y" : "N");
    g_printerr("============================================================\n\n");

    // ─── Create pipeline ───────────────────────────────────────────
    GError *err = nullptr;
    GstElement *pipeline = gst_parse_launch(pipe.toUtf8().constData(), &err);
    if (err || !pipeline) {
        g_printerr("Pipeline parse error: %s\n", err ? err->message : "nullptr");
        if (err) g_error_free(err);
        if (pipeline) gst_object_unref(pipeline);
        return nullptr;
    }

    // Encoder settings
    if (tvEn) {
        GstElement *enc = gst_bin_get_by_name(GST_BIN(pipeline), "tv_enc");
        if (enc) {
            const guint64 bps = static_cast<guint64>(cfg.tv_cam_enc_target_bps);
            g_object_set(enc, "bps", bps, "gop", static_cast<guint>(cfg.tv_cam_target_fps),
                         "bps-max", static_cast<guint64>(bps * 1.2),
                         "bps-min", static_cast<guint64>(bps * 0.8), nullptr);
            gst_object_unref(enc);
        }
    }
    if (irEn) {
        GstElement *enc = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc");
        if (enc) {
            const guint64 bps = static_cast<guint64>(cfg.ir_cam_enc_target_bps);
            g_object_set(enc, "bps", bps, "gop", static_cast<guint>(cfg.ir_cam_target_fps),
                         "bps-max", static_cast<guint64>(bps * 1.2),
                         "bps-min", static_cast<guint64>(bps * 0.8), nullptr);
            gst_object_unref(enc);
        }
    }

    // Telemetry appsrc
    if (telEn) {
        m_telemSrc = gst_bin_get_by_name(GST_BIN(pipeline), "telem_src");
        if (!m_telemSrc) g_printerr("WARNING: telem_src not found\n");
    }

    // Tracker + probes
    if (!m_trackerThread) {
        m_trackerThread = new TrackerThread(&m_frameSlot, this);
        m_trackerThread->start();
    }
    m_iniPath = (QCoreApplication::applicationDirPath() + "/tracking.ini").toStdString();

    if (tvEn) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "tv_trk_sink");
        if (sink) {
            GstPad *p = gst_element_get_static_pad(sink, "sink");
            if (p) { gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, &trackerFrameProbe, this, nullptr); gst_object_unref(p); }
            gst_object_unref(sink);
        }
    }
    if (irEn) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "ir_trk_sink");
        if (sink) {
            GstPad *p = gst_element_get_static_pad(sink, "sink");
            if (p) { gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, &trackerFrameProbe, this, nullptr); gst_object_unref(p); }
            gst_object_unref(sink);
        }
    }

    TrackerCmd tcmd; tcmd.type = TrackerCmd::REINIT; tcmd.iniPath = m_iniPath;
    m_trackerThread->submitCmd(tcmd);
    enableChan(tvEn ? TV_CH : IR_CH);

    // Camera watchdog probes (on encoder queues BEFORE mux — always see frames)
    m_tvLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    m_irLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());

    if (tvEn) {
        GstElement *q = gst_bin_get_by_name(GST_BIN(pipeline), "tv_enc_queue");
        if (q) { GstPad *p = gst_element_get_static_pad(q, "src");
            if (p) { gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, &tvFrameProbe, this, nullptr); gst_object_unref(p); }
            gst_object_unref(q); }
    }
    if (irEn) {
        GstElement *q = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc_queue");
        if (q) { GstPad *p = gst_element_get_static_pad(q, "src");
            if (p) { gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, &irFrameProbe, this, nullptr); gst_object_unref(p); }
            gst_object_unref(q); }
    }

    if (useSrt) {
        GstElement *srtSink = gst_bin_get_by_name(GST_BIN(pipeline), "mpeg_ts_srt_sink");
        if (srtSink) {
            GstPad *p = gst_element_get_static_pad(srtSink, "sink");
            if (p) {
                gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER,
                                  &GST_Wrapper::srtSinkAcceptProbe, this, nullptr);
                gst_object_unref(p);
            }
            gst_object_unref(srtSink);
        }
    }

    // Initial state: SRT suppressed (assume no listener at boot)
    m_srtSuppressWatchdog.store(true);

    // Watchdog timer
    m_mpegTsPipeline = pipeline;
    if (tvEn || irEn) {
        if (!m_mpegTsWatchdog) {
            m_mpegTsWatchdog = new QTimer(this);
            m_mpegTsWatchdog->setInterval(WATCHDOG_INTERVAL_MS);
            connect(m_mpegTsWatchdog, &QTimer::timeout, this, &GST_Wrapper::onMpegTsWatchdogTimeout);
        }
        m_mpegTsWatchdog->start();
    }

    return pipeline;
}

// ═══════════════════════════════════════════════════════════════════
//  TELEMETRY
// ═══════════════════════════════════════════════════════════════════
bool GST_Wrapper::pushTelemetry(const void *data, gsize len, GstClockTime)
{
    if (!m_telemSrc || !m_mpegTsPipeline || !m_IsActive_MPEG_TS) return false;

    GstState state, pending;
    gst_element_get_state(m_mpegTsPipeline, &state, &pending, GST_CLOCK_TIME_NONE);
    if (state < GST_STATE_PAUSED) return false;

    GstClock     *clock     = gst_element_get_clock(m_mpegTsPipeline);
    GstClockTime  base_time = gst_element_get_base_time(m_mpegTsPipeline);
    if (!clock || base_time == GST_CLOCK_TIME_NONE) {
        if (clock) gst_object_unref(clock);
        return true;
    }

    GstClockTime pts = gst_clock_get_time(clock) - base_time;
    gst_object_unref(clock);
    if (pts > (G_MAXUINT64 >> 1)) return true;

    GstBuffer *buf = gst_buffer_new_allocate(nullptr, len, nullptr);
    if (!buf) return false;
    gst_buffer_fill(buf, 0, data, len);
    GST_BUFFER_PTS(buf)      = pts;
    GST_BUFFER_DTS(buf)      = pts;
    GST_BUFFER_DURATION(buf) = GST_SECOND / 10;

    GstFlowReturn ret;
    g_signal_emit_by_name(m_telemSrc, "push-buffer", buf, &ret);
    gst_buffer_unref(buf);
    return (ret == GST_FLOW_OK);
}

void GST_Wrapper::proc_MPEG_TS_Telemetry(QByteArray datagram)
{
    if (datagram.isEmpty() || !m_IsActive_MPEG_TS) return;
    pushTelemetry(datagram.constData(), static_cast<gsize>(datagram.size()), GST_CLOCK_TIME_NONE);
}


std::string getPipelineString(GstElement *pipeline)
{
    std::stringstream ss;
    GstIterator *iter = gst_bin_iterate_elements(GST_BIN(pipeline));
    GValue item = G_VALUE_INIT;
    bool first = true;

    while (gst_iterator_next(iter, &item) == GST_ITERATOR_OK) {
        GstElement *element = GST_ELEMENT(g_value_get_object(&item));

        if (!first) ss << " ! ";
        ss << GST_ELEMENT_NAME(element);
        first = false;

        g_value_unset(&item);
    }

    gst_iterator_free(iter);
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════
//  run_GST_TVStream (non-MPEGTS standalone mode)
// ═══════════════════════════════════════════════════════════════════
GstElement *GST_Wrapper::run_GST_TVStream(QString videoDevice, QString clientIP, qint32 portUDP)
{
    // ... same as before (unchanged) ...
    GstElement* pipeline   = gst_pipeline_new("video-pipeline");
    GstElement* src        = gst_element_factory_make("v4l2src", "source");
    GstElement* capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement* queue      = gst_element_factory_make("queue", "queue");
    GstElement* tee        = gst_element_factory_make("tee", "tv_tee");
    GstElement* videorate  = gst_element_factory_make("videorate", "rate");
    GstElement* capsfilter1= gst_element_factory_make("capsfilter", "caps1");
    GstElement* encoder    = gst_element_factory_make("mpph265enc", "encoder");
    GstElement* rtppay     = gst_element_factory_make("rtph265pay", "rtppay");
    GstElement* sink       = gst_element_factory_make("udpsink", "sink");
    GstElement* queue2     = gst_element_factory_make("queue", "tv_trk_queue");
    GstElement* trk_sink   = gst_element_factory_make("fakesink", "tv_trk_sink");

    if (!pipeline||!src||!capsfilter||!queue||!tee||!videorate||!capsfilter1||!encoder||!rtppay||!sink||!queue2||!trk_sink) {
        g_printerr("Failed to create TV elements\n"); return nullptr;
    }

    GstCaps* caps = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, sm->settings_store.tv_cam_width,
                                        "height", G_TYPE_INT, sm->settings_store.tv_cam_height,
                                        "format", G_TYPE_STRING, "BGR", nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr); gst_caps_unref(caps);

    auto devBA = QByteArray(videoDevice.toLocal8Bit());
    g_object_set(src, "device", devBA.constData(), "min-buffers", 16, nullptr);
    g_object_set(queue, "leaky", 2, nullptr);
    g_object_set(queue2, "leaky", 2, nullptr);

    GstCaps* caps1 = gst_caps_new_simple("video/x-raw", "framerate", GST_TYPE_FRACTION, sm->settings_store.tv_cam_target_fps, 1, nullptr);
    g_object_set(capsfilter1, "caps", caps1, nullptr); gst_caps_unref(caps1);

    g_object_set(encoder, "bps", sm->settings_store.tv_cam_enc_target_bps, "gop", sm->settings_store.tv_cam_target_fps,
                 "bps-max", (guint64)(sm->settings_store.tv_cam_enc_target_bps*1.2),
                 "bps-min", (guint64)(sm->settings_store.tv_cam_enc_target_bps*0.8), nullptr);
    g_object_set(rtppay, "config-interval", 1, "pt", 96, "aggregate-mode", 0, nullptr);

    auto ipBA = QByteArray(clientIP.toLocal8Bit());
    g_object_set(sink, "host", ipBA.constData(), "port", portUDP, "sync", true, "async", false, nullptr);

    gst_bin_add_many(GST_BIN(pipeline), src,capsfilter,queue,tee,videorate,capsfilter1,encoder,rtppay,sink,queue2,trk_sink,nullptr);
    if (!gst_element_link_many(src,capsfilter,queue,tee,videorate,capsfilter1,encoder,rtppay,sink,nullptr)) {
        g_printerr("TV encoder link failed\n"); gst_object_unref(pipeline); return nullptr;
    }

    GstPad *ts = gst_element_get_request_pad(tee, "src_%u");
    GstPad *qs = gst_element_get_static_pad(queue2, "sink");
    if (gst_pad_link(ts, qs) != GST_PAD_LINK_OK) { gst_object_unref(ts); gst_object_unref(qs); gst_object_unref(pipeline); return nullptr; }
    gst_object_unref(qs);
    if (!gst_element_link(queue2, trk_sink)) { gst_object_unref(ts); gst_object_unref(pipeline); return nullptr; }
    gst_object_unref(ts);

    GstPad *sp = gst_element_get_static_pad(trk_sink, "sink");
    if (sp) { gst_pad_add_probe(sp, GST_PAD_PROBE_TYPE_BUFFER, &trackerFrameProbe, this, nullptr); gst_object_unref(sp); }

    GstPad *qp = gst_element_get_static_pad(queue, "src");
    if (qp) { gst_pad_add_probe(qp, GST_PAD_PROBE_TYPE_BUFFER, &tvFrameProbe, this, nullptr); gst_object_unref(qp); }

    if (!m_trackerThread) { m_trackerThread = new TrackerThread(&m_frameSlot, this); m_trackerThread->start(); }
    m_iniPath = (QCoreApplication::applicationDirPath() + "/tracking.ini").toStdString();
    TrackerCmd tc; tc.type = TrackerCmd::REINIT; tc.iniPath = m_iniPath; m_trackerThread->submitCmd(tc);
    m_activeTrackerChannel = TV_CH; m_trackerThread->setActiveChannel(TV_CH);

    std::cout << "Pipeline: " << getPipelineString(pipeline) << std::endl;

    return pipeline;
}

// ═══════════════════════════════════════════════════════════════════
//  run_GST_IRStreamISP (non-MPEGTS standalone mode)
// ═══════════════════════════════════════════════════════════════════
GstElement *GST_Wrapper::run_GST_IRStreamISP(QString videoDevice, QString clientIP, qint32 portUDP)
{
    GstElement* pipeline   = gst_pipeline_new("ir-pipeline");
    GstElement* src        = gst_element_factory_make("v4l2src", "source");
    GstElement* queue      = gst_element_factory_make("queue", "queue");
    GstElement* capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement* videorate  = gst_element_factory_make("videorate", "rate");
    GstElement* capsfilter1= gst_element_factory_make("capsfilter", "caps1");
    GstElement* g16        = gst_element_factory_make("gray16togray8", "scaler");
    GstElement* capsfilter2= gst_element_factory_make("capsfilter", "caps2");
    GstElement* convert2   = gst_element_factory_make("videoconvert", "convert2");
    GstElement* encoder    = gst_element_factory_make("mpph265enc", "encoder");
    GstElement* rtppay     = gst_element_factory_make("rtph265pay", "payloader");
    GstElement* sink       = gst_element_factory_make("udpsink", "sink");

    GstElement* tee         = gst_element_factory_make("tee", "ir_tee");
    GstElement* queue2      = gst_element_factory_make("queue", "ir_trk_queue");
    GstElement* trk_convert = gst_element_factory_make("videoconvert", "ir_trk_convert");
    GstElement* trk_caps    = gst_element_factory_make("capsfilter", "ir_trk_caps");
    GstElement* trk_sink    = gst_element_factory_make("fakesink", "ir_trk_sink");

    if (!pipeline||!src||!queue||!capsfilter||!videorate||!capsfilter1||!g16||!capsfilter2||!convert2||!encoder||!rtppay||!sink||!tee||!queue2||!trk_convert||!trk_caps||!trk_sink) {
        g_printerr("Failed to create IR elements\n"); return nullptr;
    }

    GstCaps* caps = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                        "height", G_TYPE_INT, sm->settings_store.ir_cam_height, nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr); gst_caps_unref(caps);

    auto devBA = QByteArray(videoDevice.toLocal8Bit());
    g_object_set(src, "device", devBA.constData(), "do-timestamp", TRUE, "min-buffers", 7, nullptr);
    g_object_set(queue, "leaky", 2, nullptr);

    GstCaps* caps1 = gst_caps_new_simple("video/x-raw","framerate",GST_TYPE_FRACTION,sm->settings_store.ir_cam_target_fps,1,nullptr);
    g_object_set(capsfilter1,"caps",caps1,nullptr); gst_caps_unref(caps1);

    GstCaps* caps2 = gst_caps_new_simple("video/x-raw","format",G_TYPE_STRING,"GRAY8","width",G_TYPE_INT,sm->settings_store.ir_cam_width,
                                         "height",G_TYPE_INT,sm->settings_store.ir_cam_height,"framerate",GST_TYPE_FRACTION,sm->settings_store.ir_cam_target_fps,1,nullptr);
    g_object_set(capsfilter2,"caps",caps2,nullptr); gst_caps_unref(caps2);

    GstCaps* trkOutCaps = gst_caps_new_simple("video/x-raw","format",G_TYPE_STRING,"RGB","width",G_TYPE_INT,sm->settings_store.ir_cam_width,
                                              "height",G_TYPE_INT,sm->settings_store.ir_cam_height,nullptr);
    g_object_set(trk_caps,"caps",trkOutCaps,nullptr); gst_caps_unref(trkOutCaps);

    g_object_set(encoder,"bps",sm->settings_store.ir_cam_enc_target_bps,"gop",sm->settings_store.ir_cam_target_fps,
                 "bps-max",(guint64)(sm->settings_store.ir_cam_enc_target_bps*1.2),"bps-min",(guint64)(sm->settings_store.ir_cam_enc_target_bps*0.8),nullptr);
    g_object_set(rtppay,"config-interval",1,"pt",96,"aggregate-mode",0,nullptr);

    auto ipBA = QByteArray(clientIP.toLocal8Bit());
    g_object_set(sink,"host",ipBA.constData(),"port",portUDP,"sync",true,"async",false,nullptr);

    gst_bin_add_many(GST_BIN(pipeline),src,capsfilter,queue,tee,videorate,capsfilter1,g16,capsfilter2,convert2,encoder,rtppay,sink,queue2,trk_convert,trk_caps,trk_sink,nullptr);
    if (!gst_element_link_many(src,capsfilter,queue,tee,videorate,capsfilter1,g16,capsfilter2,convert2,encoder,rtppay,sink,nullptr)) {
        g_printerr("IR encoder link failed\n"); gst_object_unref(pipeline); return nullptr;
    }

    GstPad *ts = gst_element_get_request_pad(tee,"src_%u");
    GstPad *qs = gst_element_get_static_pad(queue2,"sink");
    if (gst_pad_link(ts,qs)!=GST_PAD_LINK_OK) { gst_object_unref(ts); gst_object_unref(qs); gst_object_unref(pipeline); return nullptr; }
    gst_object_unref(qs);
    if (!gst_element_link_many(queue2,trk_convert,trk_caps,trk_sink,nullptr)) { gst_object_unref(ts); gst_object_unref(pipeline); return nullptr; }
    gst_object_unref(ts);

    GstPad *sp = gst_element_get_static_pad(trk_sink,"sink");
    if (sp) { gst_pad_add_probe(sp,GST_PAD_PROBE_TYPE_BUFFER,&trackerFrameProbe,this,nullptr); gst_object_unref(sp); }

    GstPad *qp = gst_element_get_static_pad(queue,"src");
    if (qp) { gst_pad_add_probe(qp,GST_PAD_PROBE_TYPE_BUFFER,&irFrameProbe,this,nullptr); gst_object_unref(qp); }

    return pipeline;
}

