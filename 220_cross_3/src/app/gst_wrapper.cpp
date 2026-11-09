#include "gst_wrapper.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <cerrno>
#include <fcntl.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <unistd.h>

// ═══════════════════════════════════════════════════════════════════
//  BUS CALL — called from pollBus() (NOT GSource/GMainLoop)
// ═══════════════════════════════════════════════════════════════════
gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer ptr)
{
    Q_UNUSED(bus);
    auto *self = static_cast<GST_Wrapper *>(ptr);

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
        break;

    case GST_MESSAGE_ERROR:
    {
        gchar *debug = nullptr;
        GError *error = nullptr;
        gst_message_parse_error(msg, &error, &debug);

        const gchar *srcName = GST_MESSAGE_SRC(msg)
                                   ? GST_OBJECT_NAME(GST_MESSAGE_SRC(msg))
                                   : "";

        if (g_str_has_prefix(srcName, "mpeg_ts_srt") && self)
        {
            // SRT error → restart ONLY transport pipeline; encode + cameras keep running
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (now - self->m_lastSrtLogMs.load() > GST_Wrapper::SRT_LOG_INTERVAL_MS)
            {
                g_printerr("[SRT] ERROR: %s — restart transport\n",
                           error ? error->message : "?");
                self->m_lastSrtLogMs.store(now);
            } // if (now - self->m_lastSrtLogMs.load() > GST_Wrapper::SRT_LOG_INTERVAL_MS)
            QMetaObject::invokeMethod(self, [self]()
                                      {
                                          if (self->m_IsActive_MPEG_TS)
                                              self->restartTransportPipeline();
                                      }, // lambda [self] ()
                                      Qt::QueuedConnection);
        } // if (g_str_has_prefix(srcName, "mpeg_ts_srt") && self)
        else
        {
            // Critical error (camera/encoder) → restart
            g_printerr("FATAL ERROR: %s (%s)\n",
                       error ? error->message : "?", srcName);
            if (self && self->m_mpegTsPipeline)
            {
                QMetaObject::invokeMethod(self, [self]()
                                          {
                                              self->restartMpegTsPipeline();
                                          }, // lambda [self] ()
                                          Qt::QueuedConnection);
            } // if (self && self->m_mpegTsPipeline)
        } // else

        if (debug)
            g_free(debug);
        g_error_free(error);
        break;
    } // case GST_MESSAGE_ERROR

    case GST_MESSAGE_WARNING:
    {
        GError *warning = nullptr;
        gchar *debug = nullptr;
        gst_message_parse_warning(msg, &warning, &debug);

        const gchar *srcName = GST_MESSAGE_SRC(msg)
                                   ? GST_OBJECT_NAME(GST_MESSAGE_SRC(msg))
                                   : "";
        const QString desc = warning ? QString::fromUtf8(warning->message) : QString();

        const bool isSrtIssue = g_str_has_prefix(srcName, "mpeg_ts_srt") ||
                                desc.contains("Connection timeout", Qt::CaseInsensitive);

        if (isSrtIssue && self)
        {
            // SRT warning → let srtsink auto-reconnect handle it; log-only throttled
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (now - self->m_lastSrtLogMs.load() > GST_Wrapper::SRT_LOG_INTERVAL_MS)
            {
                g_printerr("[SRT] WARNING: %s\n", desc.toUtf8().constData());
                self->m_lastSrtLogMs.store(now);
            } // if (now - self->m_lastSrtLogMs.load() > GST_Wrapper::SRT_LOG_INTERVAL_MS)
        } // if (isSrtIssue && self)
        else
        {
            g_printerr("[GST WARN] %s: %s\n", srcName, warning ? warning->message : "?");
        } // else

        if (warning)
            g_error_free(warning);
        g_free(debug);
        break;
    } // case GST_MESSAGE_WARNING

    default:
        break;
    } // switch (GST_MESSAGE_TYPE(msg))
    return TRUE;
} // bus_call

// ═══════════════════════════════════════════════════════════════════
//  BUS POLLING — drains bus via QTimer (Qt event loop compatible)
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::pollBus()
{
    for (GstElement *p : {m_mpegTsPipeline, m_transportPipeline})
    {
        if (!p)
            continue;
        GstBus *bus = gst_element_get_bus(p);
        if (!bus)
            continue;

        int count = 0;
        while (count < BUS_POLL_MAX_MSGS)
        {
            GstMessage *msg = gst_bus_pop_filtered(bus, static_cast<GstMessageType>(
                                                            GST_MESSAGE_ERROR | GST_MESSAGE_WARNING | GST_MESSAGE_EOS));
            if (!msg)
                break;

            bus_call(bus, msg, this);
            gst_message_unref(msg);
            count++;
        } // while (count < BUS_POLL_MAX_MSGS)

        gst_object_unref(bus);
    } // for (GstElement *p : {m_mpegTsPipeline, m_transportPipeline})
} // GST_Wrapper::pollBus

// ═══════════════════════════════════════════════════════════════════
//  CONSTRUCTOR / DESTRUCTOR
// ═══════════════════════════════════════════════════════════════════
GST_Wrapper::GST_Wrapper(SettingsManager *sm, QObject *parent)
    : QObject(parent), sm{sm}
{
    qDebug() << Q_FUNC_INFO;

    // Поток без parent: при зависшем (не join'нутом) потоке объект намеренно
    // утекает, вместо qFatal("QThread: Destroyed while thread is still running").
    thread = new QThread();

    connect(thread, &QThread::started, this, [this]
            {
                gst_PipelineInit();
                qDebug() << "GST Pipe init done";
            }, // lambda [this]
            Qt::DirectConnection);

    QObject::connect(thread, &QThread::finished, this, &GST_Wrapper::proc_Thread_Finished);

    moveToThread(thread);
    thread->start();
    gst_wrapper_inst = this;
} // GST_Wrapper::GST_Wrapper

// Остановка потока трекера — единая точка. quit() не используется: у TrackerThread
// собственный run()-цикл без event loop (quit() был бы no-op → поток никогда не
// останавливался, а delete работающего QThread давал qFatal/SIGABRT на выходе).
void GST_Wrapper::stopTrackerThread()
{
    if (!m_trackerThread)
        return;

    const int budget = m_shutdownMode.load(std::memory_order_relaxed) ? 1500 : 3000;
    if (m_trackerThread->stop(budget))
    {
        delete m_trackerThread;
    } // if (m_trackerThread->stop(budget))
    else
    {
        qWarning() << "[GST] tracker thread did not stop within" << budget
                   << "ms — leaking it instead of destroying a running QThread";
    } // else
    m_trackerThread = nullptr;
} // GST_Wrapper::stopTrackerThread

GST_Wrapper::~GST_Wrapper()
{
    m_IsActive_MPEG_TS = false;
    m_IsActiveTV = false;
    m_IsActiveIR = false;

    if (thread)
    {
        // Только ждём: terminate()/удаление работающего потока недопустимы.
        if (!thread->wait(5000))
        {
            qWarning() << "[GST] worker thread still running at destruction — leaking it";
        } // if (!thread->wait(5000))
        else
        {
            delete thread;
            thread = nullptr;
        } // else
    } // if (thread)

    // Гарантированно освобождаем pump-поток: удаление joinable std::thread без join
    // вызывает std::terminate. В нормальном пути это уже сделал proc_Thread_Finished —
    // здесь идемпотентная страховка для аварийного/преждевременного уничтожения.
    setPumpAppsrc(nullptr);
    stopPumpThread();

    if (gst_wrapper_inst == this)
        gst_wrapper_inst = nullptr;
} // GST_Wrapper::~GST_Wrapper

// ════════════════════════════════════════════════════════════════════
//  Graceful shutdown at process termination.
//  The wrapper lives on its own thread (moveToThread), so we cannot simply
//  delete it from the main thread like a plain worker: we must stop the
//  pipelines and quit/join the worker loop first. proc_Thread_Finished
//  (connected to QThread::finished) performs the heavy release — transport
//  pipeline, anchor pipe fds, IR/TV/MPEG-TS pipelines, tracker and watchdog —
//  so quitting the event loop deterministically frees everything.
// ════════════════════════════════════════════════════════════════════
bool GST_Wrapper::shutdownGracefully(int timeoutMs)
{
    m_shutdownMode.store(true, std::memory_order_release);

    if (!thread || !thread->isRunning())
        return true; // уже остановлен (идемпотентно) — объект можно удалять

    m_IsActive_MPEG_TS = false;
    m_IsActiveTV = false;
    m_IsActiveIR = false;

    // Остановка выполняется ВНУТРИ worker-потока: proc_shutdownAndQuit сначала гасит
    // конвейеры/трекер, и только в самом конце выходит из event loop (thread->quit()),
    // что запускает proc_Thread_Finished (транспорт + pipe fds). Порядок FIFO в очереди
    // событий гарантирует: очистка завершена ДО выхода из цикла.
    // Ранее quit() дёргался отсюда сразу: queued-слот не успевал выполниться, а wait()
    // истекал, пока worker стоял в долгих set_state(NULL) → terminate() → SIGABRT.
    QMetaObject::invokeMethod(this, "proc_shutdownAndQuit", Qt::QueuedConnection);

    if (thread->wait(timeoutMs))
    {
        g_printerr("[GST] worker thread joined cleanly\n");
        return true;
    } // if (thread->wait(timeoutMs))

    // Второй шанс: возможно слот не был доставлен — попросим цикл выйти напрямую.
    qWarning() << "[GST] worker thread not finished within" << timeoutMs
               << "ms — requesting quit";
    thread->quit();
    if (thread->wait(1000))
    {
        g_printerr("[GST] worker thread joined after quit request\n");
        return true;
    } // if (thread->wait(1000))

    // terminate() НЕ вызывается: поток, зависший внутри GStreamer/v4l2 close(), убить
    // нельзя, а удаление работающего QThread даёт qFatal("... still running") → SIGABRT.
    // Возвращаем false: вызывающий обязан завершить процесс через _exit(), все UART-порты
    // к этому моменту уже закрыты (GSP/линзы/MINI640 закрываются раньше).
    g_printerr("[GST] worker thread is stuck — caller must _exit(), do NOT delete this object\n");
    return false;
} // GST_Wrapper::shutdownGracefully

// ═══════════════════════════════════════════════════════════════════
//  PAD PROBES
// ═══════════════════════════════════════════════════════════════════
GstPadProbeReturn GST_Wrapper::trackerFrameProbe(
    GstPad *pad, GstPadProbeInfo *info, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper *>(userData);

    if (!(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_BUFFER))
        return GST_PAD_PROBE_OK;

    GstBuffer *buf = GST_PAD_PROBE_INFO_BUFFER(info);
    if (!buf)
        return GST_PAD_PROBE_OK;

    GstElement *parent = gst_pad_get_parent_element(pad);
    int ch = -1;
    if (parent)
    {
        const gchar *name = GST_OBJECT_NAME(parent);
        if (g_str_has_prefix(name, "tv_"))
            ch = 0;
        else if (g_str_has_prefix(name, "ir_"))
            ch = 1;
        gst_object_unref(parent);
    } // if (parent)

    if (ch != self->m_activeTrackerChannel)
        return GST_PAD_PROBE_OK;

    GstCaps *caps = gst_pad_get_current_caps(pad);
    if (!caps)
        return GST_PAD_PROBE_OK;

    GstVideoInfo vinfo;
    if (!gst_video_info_from_caps(&vinfo, caps))
    {
        gst_caps_unref(caps);
        return GST_PAD_PROBE_OK;
    } // if (!gst_video_info_from_caps(&vinfo, caps))
    gst_caps_unref(caps);

    GstMapInfo map;
    if (!gst_buffer_map(buf, &map, GST_MAP_READ))
        return GST_PAD_PROBE_OK;

    cv::Mat frame(static_cast<int>(vinfo.height),
                  static_cast<int>(vinfo.width), CV_8UC3, map.data);
    self->m_frameSlot.write(frame, ch);

    gst_buffer_unmap(buf, &map);
    return GST_PAD_PROBE_OK;
} // GST_Wrapper::trackerFrameProbe

GstPadProbeReturn GST_Wrapper::tvFrameProbe(GstPad *, GstPadProbeInfo *, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper *>(userData);
    self->m_tvLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    return GST_PAD_PROBE_OK;
} // GST_Wrapper::tvFrameProbe

GstPadProbeReturn GST_Wrapper::irFrameProbe(GstPad *, GstPadProbeInfo *, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper *>(userData);
    self->m_irLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    return GST_PAD_PROBE_OK;
} // GST_Wrapper::irFrameProbe

// ═══════════════════════════════════════════════════════════════════
//  Мост encode→transport: кольцо TS + pump-поток
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::initTsBridge()
{
    if (m_tsRing.availableBytes() != 0 || m_pumpRun.load())
        return; // уже инициализирован/идёт
    m_tsRing.init(TS_RING_CAPACITY_BYTES);
} // GST_Wrapper::initTsBridge

// pump-поток: читает целые пакеты из кольца и пушит в текущий appsrc.
// Если appsrc нет (транспорт остановлен/не готов) — данные НЕ читаем, кольцо
// заполняется и producer дропает старые пакеты; encode при этом свободен.
void GST_Wrapper::pumpLoop()
{
    // Один «кусок» перелива — несколько TS-пакетов, чтобы не дёргать сигнал на каждый байт.
    std::vector<uint8_t> tmp(PUMP_CHUNK);
    auto lastLogMs = QDateTime::currentMSecsSinceEpoch();
    long long pushedPkts = 0; // всего пакетов отправлено в транспорт
    long long lastCaptured = 0, lastPushedPkts = 0, lastMuxOut = 0;
    int diagTick = 0;
    // Флаг периодической диагностики [Bridge]/[Diag] из конфига (MPEG-TS.Diag_Logs).
    const bool diagLogs = sm->settings_store.mpeg_ts_diag_logs;

    while (m_pumpRun.load(std::memory_order_acquire))
    {
        m_tsRing.waitData(50);
        if (!m_pumpRun.load(std::memory_order_acquire))
            break;

        // Временная ссылка на appsrc на время пуша
        GstElement *src = nullptr;
        {
            std::lock_guard<std::mutex> lk(m_srcMutex);
            if (m_pumpAppsrc)
                src = GST_ELEMENT(gst_object_ref(m_pumpAppsrc));
        } // block
        if (!src)
        { // транспорта нет: кольцо само дропает старое
            QThread::msleep(20);
            continue;
        } // if (!src)

        bool backedUp = false;
        while (m_pumpRun.load(std::memory_order_relaxed))
        {
            // Транспорт не успевает: не копим в appsrc. Данные остаются в кольце,
            // где старое выкидывается по границе 188-байтного пакета.
            if (gst_app_src_get_current_level_bytes(GST_APP_SRC(src)) > APPSRC_MAX_BYTES)
            {
                backedUp = true;
                break;
            } // if (gst_app_src_get_current_level_bytes(GST_APP_SRC(src)) > APPSRC_MAX_BYTES)
            const std::size_t n = m_tsRing.read(tmp.data(), tmp.size());
            if (n == 0)
                break;

            GstBuffer *buf = gst_buffer_new_allocate(nullptr, n, nullptr);
            gst_buffer_fill(buf, 0, tmp.data(), n);
            // push_buffer забирает ссылку на buf
            GstFlowReturn fret = gst_app_src_push_buffer(GST_APP_SRC(src), buf);
            if (fret != GST_FLOW_OK)
            {
                backedUp = true; // flushing/eos при рестарте транспорта
                if (fret != GST_FLOW_FLUSHING && fret != GST_FLOW_EOS)
                {
                    g_printerr("[Pump] push-buffer flow=%d (transport not ready?)\n",
                               static_cast<int>(fret));
                    QMetaObject::invokeMethod(this, [this]()
                                              {
                                                  if (!m_shutdownMode.load(std::memory_order_relaxed) && m_pumpRun.load())
                                                      restartTransportPipeline();
                                              }, // lambda [this] ()
                                              Qt::QueuedConnection);
                } // if (fret != GST_FLOW_FLUSHING && fret != GST_FLOW_EOS)
                break;
            } // if (fret != GST_FLOW_OK)
            pushedPkts += static_cast<long long>(n / TsRingBuffer::TS_PKT);
        } // while (m_pumpRun.load(std::memory_order_relaxed))
        gst_object_unref(src);
        if (backedUp)
            QThread::msleep(3);

        // ── периодическая диагностика моста encode→transport (раз в ~2 с) ──
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (diagLogs && now - lastLogMs >= 2000)
        {
            const long long captured = m_tsCapturedBytes.load(std::memory_order_relaxed);
            const long long muxOut = m_muxOutPkts.load(std::memory_order_relaxed);
            g_printerr("[Bridge] mux=%lldpkt/s cap=%lldB/s push=%lldpkt/s avail=%llu drop=%lld src=%s\n",
                       (muxOut - lastMuxOut) / 2,
                       (captured - lastCaptured) / 2,
                       (pushedPkts - lastPushedPkts) / 2,
                       static_cast<unsigned long long>(m_tsRing.availableBytes()),
                       m_tsRing.droppedPackets(),
                       (m_pumpAppsrc ? "Y" : "N"));
            lastLogMs = now;
            lastCaptured = captured;
            lastPushedPkts = pushedPkts;
            lastMuxOut = muxOut;

            // Раз в ~6 с — подробный дамп encode-пайплайна (main-поток, non-blocking).
            // Покажет: дошёл ли pipeline до PLAYING и какие pads mpegtsmux пусты.
            if (++diagTick % 3 == 0)
            {
                QMetaObject::invokeMethod(this, [this]()
                                          {
                                              mpegtsDiagDump();
                                          }, // lambda [this] ()
                                          Qt::QueuedConnection);
            } // if (++diagTick % 3 == 0)
        } // if (diagLogs && now - lastLogMs >= 2000)
    } // while (m_pumpRun.load(std::memory_order_acquire))

} // GST_Wrapper::pumpLoop

void GST_Wrapper::startPumpThread()
{
    if (m_pumpRun.exchange(true))
        return;           // уже запущен
    m_tsRing.resetStop(); // сброс флага stop после предыдущего stopPumpThread
    m_pumpThread = std::thread([this]()
                               {
                                   pumpLoop();
                               }); // lambda [this] ()
} // GST_Wrapper::startPumpThread

void GST_Wrapper::stopPumpThread()
{
    if (!m_pumpRun.exchange(false))
    {
        // Флаг уже сброшен, но поток мог быть создан — гарантированно join.
        if (m_pumpThread.joinable())
            m_pumpThread.join();
        return;
    } // if (!m_pumpRun.exchange(false))
    m_tsRing.requestStop();
    if (m_pumpThread.joinable())
        m_pumpThread.join();
} // GST_Wrapper::stopPumpThread

void GST_Wrapper::setPumpAppsrc(GstElement *src)
{
    std::lock_guard<std::mutex> lk(m_srcMutex);
    m_pumpAppsrc = src; // владение остаётся у transport-пайплайна
} // GST_Wrapper::setPumpAppsrc

// Producer-side: вызывается из streaming-потока encode на sink-pad fakesink.
// Копирует TS-буфер в кольцо и ВСЕГДА возвращает OK → encode никогда не ждёт.
// в классе: static GstPadProbeReturn mpegtsBridgeProbe(GstPad*, GstPadProbeInfo*, gpointer);
GstPadProbeReturn GST_Wrapper::mpegtsBridgeProbe(GstPad *, GstPadProbeInfo *info, gpointer ud)
{
    auto *self = static_cast<GST_Wrapper *>(ud);

    auto feed = [self](GstBuffer *buf)
    {
        GstMapInfo mi;
        if (gst_buffer_map(buf, &mi, GST_MAP_READ))
        {
            self->m_tsRing.write(mi.data, mi.size); // доступ к private есть
            self->m_tsCapturedBytes.fetch_add(static_cast<long long>(mi.size),
                                              std::memory_order_relaxed);
            gst_buffer_unmap(buf, &mi);
        } // if (gst_buffer_map(buf, &mi, GST_MAP_READ))
    }; // lambda [self] (GstBuffer *buf)

    if (info->type & GST_PAD_PROBE_TYPE_BUFFER_LIST)
    {
        GstBufferList *list = GST_PAD_PROBE_INFO_BUFFER_LIST(info);
        const guint n = gst_buffer_list_length(list);
        for (guint i = 0; i < n; ++i)
            feed(gst_buffer_list_get(list, i));
    } // if (info->type & GST_PAD_PROBE_TYPE_BUFFER_LIST)
    else if (info->type & GST_PAD_PROBE_TYPE_BUFFER)
    {
        feed(GST_PAD_PROBE_INFO_BUFFER(info));
    } // if (info->type & GST_PAD_PROBE_TYPE_BUFFER)
    return GST_PAD_PROBE_OK;
} // GST_Wrapper::mpegtsBridgeProbe

// Диагностика: считает выходящие TS-пакеты на src-pad mpegtsmux. Ничего не меняет,
// всегда OK. Позволяет по логу [Bridge] отличить «mux молчит» от «probe не срабатывает».
// mpegtsmux выдаёт данные GstBuffer'ами со множеством 188-байтных пакетов либо
// GstBufferList'ом — считаем фактическое число пакетов (gst_buffer_get_size дешёвый,
// без map → безопасно в streaming-потоке), чтобы mux≈push (pkt/s) было сопоставимо.
GstPadProbeReturn GST_Wrapper::muxOutProbe(GstPad *, GstPadProbeInfo *info, gpointer userData)
{
    auto *self = static_cast<GST_Wrapper *>(userData);
    const GstPadProbeType type = GST_PAD_PROBE_INFO_TYPE(info);
    long long pkts = 0;

    if (type & GST_PAD_PROBE_TYPE_BUFFER)
    {
        GstBuffer *buf = GST_PAD_PROBE_INFO_BUFFER(info);
        if (buf)
            pkts = static_cast<long long>(gst_buffer_get_size(buf) / TsRingBuffer::TS_PKT);
    } // if (type & GST_PAD_PROBE_TYPE_BUFFER)
    else if (type & GST_PAD_PROBE_TYPE_BUFFER_LIST)
    {
        GstBufferList *list = GST_PAD_PROBE_INFO_BUFFER_LIST(info);
        const guint n = gst_buffer_list_length(list);
        for (guint i = 0; i < n; ++i)
        {
            GstBuffer *b = gst_buffer_list_get(list, i);
            if (b)
                pkts += static_cast<long long>(gst_buffer_get_size(b) / TsRingBuffer::TS_PKT);
        } // for (guint i = 0; i < n; ++i)
    } // if (type & GST_PAD_PROBE_TYPE_BUFFER_LIST)

    if (pkts > 0)
        self->m_muxOutPkts.fetch_add(pkts, std::memory_order_relaxed);
    return GST_PAD_PROBE_OK;
} // GST_Wrapper::muxOutProbe

// Считает входящие буферы на sink-pad'ах mpegtsmux. user_data — atomic<long long>*.
GstPadProbeReturn GST_Wrapper::muxSinkCountProbe(GstPad *, GstPadProbeInfo *info, gpointer userData)
{
    if (GST_PAD_PROBE_INFO_TYPE(info) & (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST))
    {
        auto *ctr = static_cast<std::atomic<long long> *>(userData);
        if (ctr)
            ctr->fetch_add(1, std::memory_order_relaxed);
    } // if (GST_PAD_PROBE_INFO_TYPE(info) & (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST))
    return GST_PAD_PROBE_OK;
} // GST_Wrapper::muxSinkCountProbe

// Диагностика encode-пайплайна. Вызывается из main-потока (через invokeMethod).
// get_state с timeout=0 — неблокирующий: вернёт текущий фактический state без ожидания.
void GST_Wrapper::mpegtsDiagDump()
{
    if (!sm->settings_store.mpeg_ts_diag_logs)
        return; // периодическая диагностика выключена в конфиге
    if (!m_mpegTsPipeline)
    {
        g_printerr("[Diag] no encode pipeline\n");
        return;
    } // if (!m_mpegTsPipeline)

    GstState cur = GST_STATE_VOID_PENDING, pend = GST_STATE_VOID_PENDING;
    const GstStateChangeReturn scr =
        gst_element_get_state(m_mpegTsPipeline, &cur, &pend, 0);
    g_printerr("[Diag] encode state=%s pending=%s change=%d\n",
               gst_element_state_get_name(cur),
               (pend != GST_STATE_VOID_PENDING) ? gst_element_state_get_name(pend) : "-",
               static_cast<int>(scr));

    GstElement *mux = gst_bin_get_by_name(GST_BIN(m_mpegTsPipeline), "mux");
    if (!mux)
    {
        g_printerr("[Diag] mux not found\n");
        return;
    } // if (!mux)

    // Перечисляем sink-pad'ы mpegtsmux: имена (sink_256/257/512...), active, caps.
    QString pads;
    GstIterator *it = gst_element_iterate_sink_pads(mux);
    GValue v = G_VALUE_INIT;
    while (gst_iterator_next(it, &v) == GST_ITERATOR_OK)
    {
        GstPad *p = GST_PAD(g_value_get_object(&v));
        const gboolean active = gst_pad_is_active(p);
        GstCaps *c = gst_pad_get_current_caps(p);
        gchar *cs = c ? gst_caps_to_string(c) : g_strdup("(no caps — ждёт данные)");
        pads += QString("\n    %1 active=%2 caps=%3")
                    .arg(GST_OBJECT_NAME(p))
                    .arg(active ? "Y" : "N")
                    .arg(cs);
        if (c)
            gst_caps_unref(c);
        g_free(cs);
        g_value_unset(&v);
    } // while (gst_iterator_next(it, &v) == GST_ITERATOR_OK)
    gst_iterator_free(it);

    GstPad *msrc = gst_element_get_static_pad(mux, "src");
    const gboolean linked = msrc && gst_pad_is_linked(msrc);
    if (msrc)
        gst_object_unref(msrc);
    g_printerr("[Diag] mux sink-pads:%s\n[Diag] mux.src linked=%s in(256/257/klv)=%lld/%lld/%lld captured_total=%lldB mux_out_total=%lldpkt\n",
               pads.toUtf8().constData(), linked ? "Y" : "N",
               m_muxIn256.load(std::memory_order_relaxed),
               m_muxIn257.load(std::memory_order_relaxed),
               m_muxInKlv.load(std::memory_order_relaxed),
               m_tsCapturedBytes.load(std::memory_order_relaxed),
               m_muxOutPkts.load(std::memory_order_relaxed));

    gst_object_unref(mux);
} // GST_Wrapper::mpegtsDiagDump

bool GST_Wrapper::startTransportPipeline()
{
    if (m_transportPipeline)
        return true; // already running

    const auto &cfg = sm->settings_store;

    // Build transport pipeline: appsrc(pump←кольцо) -> video/mpegts caps -> srtsink | rtpmp2tpay ! udpsink
    const bool useSrt = cfg.mpeg_ts_use_srt;
    QString pipeStr;

    // Транспорт тянет данные из pump-потока через appsrc (push-buffer), а не из
    // kernel-pipe. Это снимает backpressure с encode: если SRT стоит, блокируется
    // только pump, кольцо заполняется и producer дропает старые целые пакеты.
    // max-bytes задаёт «подушку» внутри appsrc — до её заполнения push не тормозит pump.
    pipeStr = QString("appsrc name=transport_src is-live=true format=time do-timestamp=true "
                      "block=false max-bytes=%1")
                  .arg(APPSRC_MAX_BYTES) // 128 KiB — pump не пушит дальше, srtsink дропает без клиента
              + " ! video/mpegts, ts-packet-size=188";

    // Очередь перед сетевым элементом — только для thread decoupling (queue
    // порождает отдельный streaming-поток, отделяющий appsrc от srtsink).
    // НЕ leaky: backpressure на appsrc создаётся штатно — pump останавливается
    // по проверке gst_app_src_get_current_level_bytes(), а кольцо дропает старые.
    // leaky=downstream здесь ДРОПАЛ НОВЫЕ (свежие) буферы при burst'ах pump,
    // что вызывало периодические потери на приёмнике даже при хорошем линке.
    const QString leakyQ =
        " ! queue max-size-buffers=0 max-size-bytes=262144 max-size-time=0";

    if (useSrt)
    {
        const int srtLatencyMs = qBound(20, cfg.mpeg_ts_srt_latency_ms, 5000);
        const bool srtTlpktdrop = cfg.mpeg_ts_srt_tlpktdrop;
        const int srtPayload = cfg.mpeg_ts_srt_ts_packet_count * 188;

        QString srtUri = QString("srt://%1:%2?mode=caller&latency=%3&payloadsize=%4&tlpktdrop=%5")
                             .arg(cfg.ip_host)
                             .arg(cfg.mpeg_ts_port)
                             .arg(srtLatencyMs)
                             .arg(srtPayload)
                             .arg(srtTlpktdrop ? "true" : "false");
        if (cfg.mpeg_ts_srt_fec_enabled)
        {
            srtUri += QString("&packetfilter=fec,cols:%1,rows:%2,layout:%3,arq:%4")
                          .arg(qBound(2, cfg.mpeg_ts_srt_fec_cols, 255))
                          .arg(qBound(1, cfg.mpeg_ts_srt_fec_rows, 255))
                          .arg(cfg.mpeg_ts_srt_fec_layout.trimmed().toLower())
                          .arg(cfg.mpeg_ts_srt_fec_arq.trimmed().toLower());
        } // if (cfg.mpeg_ts_srt_fec_enabled)

        // wait-for-connection=false: пока клиента нет, данные выбрасываются, а не копятся
        pipeStr += leakyQ;
        pipeStr += QString(" ! srtsink name=mpeg_ts_srt_sink uri=\"%1\" latency=%2"
                           " auto-reconnect=true wait-for-connection=false sync=false async=false")
                       .arg(srtUri)
                       .arg(srtLatencyMs);
    } // if (useSrt)
    else
    {
        pipeStr += leakyQ;
        pipeStr += QString(" ! rtpmp2tpay pt=33 mtu=1328"
                           " ! udpsink host=%1 port=%2 sync=false async=false")
                       .arg(cfg.ip_host)
                       .arg(cfg.mpeg_ts_port);
    } // else

    GError *err = nullptr;
    m_transportPipeline = gst_parse_launch(pipeStr.toUtf8().constData(), &err);
    if (err || !m_transportPipeline)
    {
        g_printerr("[Transport] parse error: %s\n", err ? err->message : "?");
        if (err)
            g_error_free(err);
        if (m_transportPipeline)
        {
            gst_object_unref(m_transportPipeline);
            m_transportPipeline = nullptr;
        } // if (m_transportPipeline)
        return false;
    } // if (err || !m_transportPipeline)

    // Явные caps на appsrc исключают неопределённость negotiation с srtsink/rtpmp2tpay:
    // выставляем ДО перевода в PLAYING, чтобы первый push шёл уже с согласованными
    // video/mpegts(systemstream=true, packetsize=188).
    GstElement *transport_src = gst_bin_get_by_name(GST_BIN(m_transportPipeline), "transport_src");
    if (transport_src)
    {
        GstCaps *tcaps = gst_caps_new_simple("video/mpegts",
                                             "systemstream", G_TYPE_BOOLEAN, TRUE,
                                             "packetsize", G_TYPE_INT, 188, nullptr);
        g_object_set(transport_src, "caps", tcaps, nullptr);
        gst_caps_unref(tcaps);
    } // if (transport_src)

    // На GStreamer >= 1.20 appsrc сам выбрасывает старое при переполнении;
    // на более старых выручает проверка уровня в pumpLoop.
    if (transport_src && g_object_class_find_property(G_OBJECT_GET_CLASS(transport_src), "leaky-type"))
        gst_util_set_object_arg(G_OBJECT(transport_src), "leaky-type", "downstream");

    // Сигнал caller-added есть только в listener-режиме srtsink (GStreamer >= 1.18).
    // В caller-режиме он не срабатывает, но проверка g_signal_lookup безопасна.
    if (useSrt)
    {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(m_transportPipeline), "mpeg_ts_srt_sink");
        if (sink)
        {
            if (g_signal_lookup("caller-added", G_OBJECT_TYPE(sink)))
                g_signal_connect(sink, "caller-added",
                                 G_CALLBACK(&GST_Wrapper::onSrtCallerAdded), this);
            gst_object_unref(sink);
        } // if (sink)
    } // if (useSrt)

    gst_element_set_state(m_transportPipeline, GST_STATE_PLAYING);

    // Подключаем pump к новому appsrc и сбрасываем устаревший бэклолог кольца:
    // после простоя (SRT был недоступен) в кольце лежат «протухшие» кадры —
    // начинаем вещание со свежих, чтобы не затапливать SRT старым burst'ом.
    if (transport_src)
    {
        m_tsRing.clear();
        // Caller-режим: сигнала caller-added нет (он только в listener). Сбрасываем
        // устаревшее кольцо и сразу просим IDR у энкодеров, чтобы после (ре)коннекта
        // клиент получил картинку с ключевого кадра, не дожидаясь конца gop.
        requestKeyframes();
        setPumpAppsrc(transport_src);
        gst_object_unref(transport_src); // владение — у пайплайна; pump держит свой ref
    } // if (transport_src)
    else
    {
        g_printerr("[Transport] appsrc 'transport_src' not found — pump will idle\n");
    } // else

    m_transportRestartCount = 0; // reset backoff on successful start
    g_printerr("[Transport] Pipeline started (%s): host=%s port=%d latency_ms=%d tlpktdrop=%d "
               "ts_pkt_cnt=%d payload=%dB fec=%d(cols=%d,rows=%d,layout=%s,arq=%s)\n",
               useSrt ? "SRT" : "RTP/UDP",
               cfg.ip_host.toLocal8Bit().constData(), cfg.mpeg_ts_port,
               qBound(20, cfg.mpeg_ts_srt_latency_ms, 5000),
               cfg.mpeg_ts_srt_tlpktdrop ? 1 : 0,
               cfg.mpeg_ts_srt_ts_packet_count, cfg.mpeg_ts_srt_ts_packet_count * 188,
               cfg.mpeg_ts_srt_fec_enabled ? 1 : 0,
               qBound(2, cfg.mpeg_ts_srt_fec_cols, 255),
               qBound(1, cfg.mpeg_ts_srt_fec_rows, 255),
               cfg.mpeg_ts_srt_fec_layout.trimmed().toLower().toLocal8Bit().constData(),
               cfg.mpeg_ts_srt_fec_arq.trimmed().toLower().toLocal8Bit().constData());
    return true;
} // GST_Wrapper::startTransportPipeline

void GST_Wrapper::stopTransportPipeline()
{
    if (!m_transportPipeline)
        return;

    // Сначала отвязываем pump, чтобы он перестал пушить в уходящий appsrc, и даём
    // завершиться уже начатому push (pump держит свою временную ссылку на элемент).
    setPumpAppsrc(nullptr);
    usleep(20 * 1000);

    gst_element_set_state(m_transportPipeline, GST_STATE_NULL);
    gst_object_unref(m_transportPipeline);
    m_transportPipeline = nullptr;
    g_printerr("[Transport] Pipeline stopped\n");
} // GST_Wrapper::stopTransportPipeline

// ───────────────────────────────────────────────────────────────────
// 3. Подключение клиента: сброс кольца + запрос ключевого кадра
// ───────────────────────────────────────────────────────────────────
// Вызывается из потока libsrt: только потокобезопасные действия.
void GST_Wrapper::onSrtCallerAdded(GstElement *, gint, gpointer, gpointer ud)
{
    auto *self = static_cast<GST_Wrapper *>(ud);
    self->m_tsRing.clear(); // выкинуть всё, что накопилось до подключения
    QMetaObject::invokeMethod(self, [self]
                              {
                                  self->requestKeyframes();
                              }, // lambda [self]
                              Qt::QueuedConnection);
} // GST_Wrapper::onSrtCallerAdded

// Если mpph265enc не поддерживает событие, ничего страшного:
// клиент дождётся ближайшего IDR.
void GST_Wrapper::requestKeyframes()
{
    if (!m_mpegTsPipeline)
        return;
    for (const char *name : {"tv_enc", "ir_enc"})
    {
        GstElement *enc = gst_bin_get_by_name(GST_BIN(m_mpegTsPipeline), name);
        if (!enc)
            continue;
        gst_element_send_event(enc,
                               gst_video_event_new_upstream_force_key_unit(GST_CLOCK_TIME_NONE, TRUE, 0));
        gst_object_unref(enc);
    } // for (const char *name : {"tv_enc", "ir_enc"})
} // GST_Wrapper::requestKeyframes

void GST_Wrapper::restartTransportPipeline()
{
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return; // не рестартуем на выходе
    stopTransportPipeline();

    // Exponential backoff: 1s, 2s, 4s, 8s ... capped at 30s
    const int delay = qMin(1000 * (1 << qMin(m_transportRestartCount, 5)),
                           TRANSPORT_RESTART_MAX_DELAY_MS);
    ++m_transportRestartCount;
    g_printerr("[Transport] Scheduling restart in %d ms (attempt %d)\n",
               delay, m_transportRestartCount);

    QTimer::singleShot(delay, this, [this]()
                       {
                           if (m_IsActive_MPEG_TS)
                               startTransportPipeline();
                       }); // lambda [this] ()
} // GST_Wrapper::restartTransportPipeline

// ═══════════════════════════════════════════════════════════════════
//  Tracker / Channel management
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::enableChan(int ch)
{
    if (ch < 0 || ch > 1 || !m_trackerThread)
        return;

    const bool wasActive = m_activeTrackerChannel >= 0 && m_activeTrackerChannel != ch;

    // ═══ ШАГ 1: Немедленно обнулить результаты ═══════════════════
    // Теперь proc_getActiveLockTracking() → active=false, locked=false
    // GyroPlatformHandler прекратит управление платформой СРАЗУ.
    m_trackerThread->forceResetResults();

    // ═══ ШАГ 2: Переключить канал приёма кадров ══════════════════
    m_chanEnabled[1 - ch] = false;
    m_chanEnabled[ch] = true;
    m_activeTrackerChannel = ch;
    m_trackerThread->setActiveChannel(ch);

    // ═══ ШАГ 3: Остановить и пересоздать трекер в потоке ═════════
    TrackerCmd cmd;
    if (wasActive)
    {
        cmd.type = TrackerCmd::SWITCH; // stopTracking + reset + ini
        cmd.iniPath = m_iniPath;
    } // if (wasActive)
    else
    {
        cmd.type = TrackerCmd::REINIT;
        cmd.iniPath = m_iniPath;
    } // else
    m_trackerThread->submitCmd(cmd);

    qInfo() << "[Tracker]" << (ch == TV_CH ? "TV" : "IR")
            << "enabled" << (wasActive ? "(switched)" : "(initial)");
} // GST_Wrapper::enableChan

void GST_Wrapper::disableChan(int ch)
{
    if (ch < 0 || ch > 1)
        return;
    m_chanEnabled[ch] = false;

    if (m_activeTrackerChannel == ch && m_trackerThread)
    {
        // Немедленно — результаты невалидны
        m_trackerThread->forceResetResults();

        // Потом — остановить трекер в его потоке
        TrackerCmd cmd;
        cmd.type = TrackerCmd::STOP;
        m_trackerThread->submitCmd(cmd);
        m_trackerThread->setActiveChannel(-1);
        m_activeTrackerChannel = -1;
    } // if (m_activeTrackerChannel == ch && m_trackerThread)
} // GST_Wrapper::disableChan

void GST_Wrapper::proc_Switch_Channel(quint32 channel, quint8 en)
{
    int ch = (channel == CH_TRACKER_TV) ? TV_CH : (channel == CH_TRACKER_IR) ? IR_CH
                                                                             : -1;
    if (ch < 0)
        return;

    if (en)
    {
        enableChan(ch);
    } // if (en)
    else
    {
        disableChan(ch);
    } // else
} // GST_Wrapper::proc_Switch_Channel

void GST_Wrapper::proc_getOutObject()
{
    if (!m_trackerThread)
        return;
    auto r = m_trackerThread->objectRect();
    emit sig_rdyOutObject(r.cx, r.cy, r.w, r.h);
} // GST_Wrapper::proc_getOutObject

void GST_Wrapper::proc_getOutRoi()
{
    if (!m_trackerThread)
        return;
    auto r = m_trackerThread->roiRect();
    emit sig_rdyOutRoi(r.cx, r.cy, r.w, r.h);
} // GST_Wrapper::proc_getOutRoi

void GST_Wrapper::proc_getActiveLockTracking()
{
    if (!m_trackerThread)
        return;
    emit sig_rdyActiveLockTracking(m_trackerThread->isActive(), m_trackerThread->isLockedOn());
} // GST_Wrapper::proc_getActiveLockTracking

void GST_Wrapper::proc_setTargetObject(uint cx, uint cy, uint w, uint h)
{
    if (!m_trackerThread)
        return;

    // Защита от underflow: центр должен быть не меньше половины размера
    if (cx < w / 2 || cy < h / 2)
    {
        std::cerr << "[GST_Wrapper] proc_setTargetObject: center too close to edge ("
                  << cx << "," << cy << " w=" << w << " h=" << h << ")" << std::endl;
        return;
    } // if (cx < w / 2 || cy < h / 2)

    TrShWrapper::Rect rect;
    rect.cx = static_cast<unsigned short>(cx - (w / 2));
    rect.cy = static_cast<unsigned short>(cy - (h / 2));
    rect.w = static_cast<unsigned short>(w);
    rect.h = static_cast<unsigned short>(h);

    TrackerCmd cmd;
    cmd.type = TrackerCmd::SET_TARGET;
    cmd.target = rect;
    m_trackerThread->submitCmd(cmd);
} // GST_Wrapper::proc_setTargetObject

void GST_Wrapper::proc_setStopTracking()
{
    if (!m_trackerThread)
        return;
    TrackerCmd cmd;
    cmd.type = TrackerCmd::STOP;
    m_trackerThread->submitCmd(cmd);
} // GST_Wrapper::proc_setStopTracking

void GST_Wrapper::proc_setRoiExt(double ext)
{
    Q_UNUSED(ext);
} // GST_Wrapper::proc_setRoiExt

// ═══════════════════════════════════════════════════════════════════
//  FPS handlers (non-MPEGTS mode)
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::handleFpsUpdateTV(double fps, bool active_stream)
{
    if (fps < 0.1)
    {
        if (m_tv_wd < 10)
        {
            m_tv_wd++;
            qDebug() << "TV stream stalled. WD=" << m_tv_wd;
        } // if (m_tv_wd < 10)
        else
        {
            qDebug() << "TV stream cannot resume. Restart...";
            emit sig_RstTVPipeline();
            m_tv_wd = 0;
        } // else
    } // if (fps < 0.1)
    else
    {
        qDebug() << "TV FPS:" << fps << "active:" << active_stream;
        m_tv_wd = 0;
    } // else
} // GST_Wrapper::handleFpsUpdateTV

void GST_Wrapper::handleFpsUpdateIR(double fps, bool active_stream)
{
    if (fps < 0.1)
    {
        if (m_ir_wd < 10)
        {
            m_ir_wd++;
            qDebug() << "IR stream stalled. WD=" << m_ir_wd;
        } // if (m_ir_wd < 10)
        else
        {
            qDebug() << "IR stream cannot resume. Restart...";
            emit sig_RstIRPipeline();
            m_ir_wd = 0;
        } // else
    } // if (fps < 0.1)
    else
    {
        qDebug() << "IR FPS:" << fps << "active:" << active_stream;
        m_ir_wd = 0;
    } // else
} // GST_Wrapper::handleFpsUpdateIR

// ═══════════════════════════════════════════════════════════════════
//  CAMERA CHECK HELPERS
// ═══════════════════════════════════════════════════════════════════
static bool v4l2CtlHasFrame(const QString &device, int width, int height,
                            const QString &pixelformat, int timeoutMs)
{
    QProcess process;
    QStringList args;
    args << "-d" << device;
    if (width > 0 && height > 0)
        args << "--set-fmt-video=" + QString("width=%1,height=%2,pixelformat=%3")
                                         .arg(width)
                                         .arg(height)
                                         .arg(pixelformat);
    args << "--stream-mmap=3" << "--stream-count=1" << "--stream-to=/dev/null";

    process.start("v4l2-ctl", args);
    if (!process.waitForStarted(1000))
        return false;
    if (!process.waitForFinished(timeoutMs))
    {
        process.kill();
        process.waitForFinished();
        return false;
    } // if (!process.waitForFinished(timeoutMs))
    return (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0);
} // v4l2CtlHasFrame

static bool v4l2CtlCheckWithRetries(const QString &name, const QString &device,
                                    int w, int h, const QString &fmt,
                                    int firstTo, int retryTo, int retries)
{
    for (int i = 0; i <= retries; ++i)
    {
        g_printerr("[CAM CHECK] %s attempt %d/%d\n", name.toUtf8().constData(), i + 1, retries + 1);
        if (v4l2CtlHasFrame(device, w, h, fmt, i == 0 ? firstTo : retryTo))
            return true;
    } // for (int i = 0; i <= retries; ++i)
    g_printerr("[CAM CHECK] %s FAILED\n", name.toUtf8().constData());
    return false;
} // v4l2CtlCheckWithRetries

// ═══════════════════════════════════════════════════════════════════
//  WATCHDOG (MPEG-TS)
//
//  Watchdog checks ENCODE pipeline health (state + camera staleness).
//  Transport is independent — its failure never triggers encode restart.
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::onMpegTsWatchdogTimeout()
{
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return; // на выходе ничего не рестартим
    if (!m_mpegTsPipeline)
        return;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    GstState current = GST_STATE_VOID_PENDING;
    gst_element_get_state(m_mpegTsPipeline, &current, nullptr, 0);

    if (current != GST_STATE_PLAYING)
    {
        qWarning() << "[Watchdog] Encode pipeline not PLAYING — restart";
        restartMpegTsPipeline();
        return;
    } // if (current != GST_STATE_PLAYING)

    const bool tvStale = m_tvEnInPipeline && !m_tvUseTestSrc &&
                         (now - m_tvLastFrameMs.load() > WATCHDOG_TIMEOUT_MS);
    const bool irStale = m_irEnInPipeline && !m_irUseTestSrc &&
                         (now - m_irLastFrameMs.load() > WATCHDOG_TIMEOUT_MS);

    if (tvStale || irStale)
    {
        qWarning() << "[Watchdog] Camera stale tv=" << tvStale << "ir=" << irStale;
        restartMpegTsPipeline();
    } // if (tvStale || irStale)
} // GST_Wrapper::onMpegTsWatchdogTimeout

// ═══════════════════════════════════════════════════════════════════
//  FORCE STOP PIPELINE HELPER
// ═══════════════════════════════════════════════════════════════════
static bool forceStopPipeline(GstElement *pipeline, gint timeout_ms = 3000)
{
    if (!pipeline)
        return true;

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
    while (!done)
    {
        switch (gst_iterator_next(pads, &item))
        {
        case GST_ITERATOR_OK:
        {
            GstPad *pad = GST_PAD(g_value_get_object(&item));
            gst_pad_send_event(pad, gst_event_new_flush_start());
            gst_pad_send_event(pad, gst_event_new_flush_stop(TRUE));
            g_value_unset(&item);
            break;
        } // case GST_ITERATOR_OK
        case GST_ITERATOR_DONE:
            done = TRUE;
            break;
        case GST_ITERATOR_RESYNC:
            gst_iterator_resync(pads);
            break;
        case GST_ITERATOR_ERROR:
            done = TRUE;
            break;
        } // switch (gst_iterator_next(pads, &item))
    } // while (!done)
    g_value_unset(&item);
    gst_iterator_free(pads);

    ret = gst_element_get_state(pipeline, &current, nullptr, timeout_ms * GST_MSECOND);
    return (ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_NO_PREROLL);

} // forceStopPipeline

// ═══════════════════════════════════════════════════════════════════
//  PIPELINE INIT / START / STOP / RESTART
// ═══════════════════════════════════════════════════════════════════
void GST_Wrapper::gst_PipelineInit()
{
    emit sig_STREAM_RDY();

    if (sm->settings_store.mpeg_ts_en)
    {
        gst_Start_MPEG_TS_Pipeline();
    } // if (sm->settings_store.mpeg_ts_en)
    else
    {

        connect(this, &GST_Wrapper::sig_RstIRPipeline, this, &GST_Wrapper::proc_RestartIRPipeline);

        connect(this, &GST_Wrapper::sig_RstTVPipeline, this, &GST_Wrapper::proc_RestartTVPipeline);

        gst_StartTVPipeline();
        gst_StartIRPipeline();
    } // else
    emit sig_STREAM_RDY();
} // GST_Wrapper::gst_PipelineInit

void GST_Wrapper::serviceIspCtrl(const char *s_action = "restart")
{
    QProcess p;
    p.start("sudo", {"-n", "/usr/sbin/service", "isp-goen220", s_action});
    if (!p.waitForFinished(10000))
    {
        p.kill();
        g_printerr("[ISP] %s timeout\n", s_action);
    } // if (!p.waitForFinished(10000))
    else if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0)
    {
        g_printerr("[ISP] %s failed (%d): %s\n", s_action, p.exitCode(),
                   p.readAllStandardError().constData());
    } // if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0)

    // Ждём, пока узел IR снова станет доступен (до ~6 с).
    const QByteArray dev = sm->settings_store.ir_cam_isp_video_device.toLocal8Bit();
    for (int i = 0; i < 30 && access(dev.constData(), R_OK | W_OK) != 0; ++i)
        QThread::msleep(200);
} // GST_Wrapper::serviceIspCtrl

void GST_Wrapper::gst_Start_MPEG_TS_Pipeline()
{
    // На завершении процесса конвейеры не поднимаем: иначе watchdog/отложенные
    // singleShot'ы возродят захват уже после proc_stopCapture и удержание /dev/video*.
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return;

    // Захват снова активен: сбрасываем флаг остановки, чтобы последующий
    // proc_stopCapture в runtime-сценариях не превратился в no-op.
    m_captureStopped = false;

    // Не плодим второй encode pipeline/fdsink при гонке retry/restart/watchdog.
    // Легитимный restart обнуляет m_mpegTsPipeline заранее, поэтому сюда не попадёт.
    if (m_mpegTsPipeline)
        return;

    serviceIspCtrl("restart");

    m_mpegTsPipeline = run_GST_StreamMPEG_TS(
        sm->settings_store.tv_cam_video_device,
        sm->settings_store.ir_cam_isp_video_device,
        sm->settings_store.ip_host,
        sm->settings_store.mpeg_ts_port);

    if (!m_mpegTsPipeline)
    {
        g_printerr("[Encode] Failed to create MPEG-TS pipeline (camera not ready?)\n");
        // Устройство камеры могло быть перезагружено/ещё не готово. Не бросаем конвейер
        // мёртвым: watchdog «немой» без pipeline, поэтому планируем повторный запуск.
        // Труба и транспорт остаются целыми — данные польются, как только ISP вернётся.
        if (!m_shutdownMode.load(std::memory_order_relaxed) && !m_encodeStartRetryPending)
        {
            m_encodeStartRetryPending = true;
            QTimer::singleShot(ENCODE_START_RETRY_MS, this, [this]()
                               {
                                   m_encodeStartRetryPending = false;
                                   if (m_shutdownMode.load(std::memory_order_relaxed))
                                       return;
                                   if (m_IsActive_MPEG_TS && !m_mpegTsPipeline)
                                       gst_Start_MPEG_TS_Pipeline();
                               }); // lambda [this] ()
        } // if (!m_shutdownMode.load(std::memory_order_relaxed) && !m_encodeStartRetryPending)
        return;
    } // if (!m_mpegTsPipeline)

    // Успешный запуск — сбрасываем флаг повтора, чтобы не блокировать будущие попытки.
    m_encodeStartRetryPending = false;

    // Bus polling (NO gst_bus_add_watch — requires GLib loop which is absent)
    if (!m_busPollTimer)
    {
        m_busPollTimer = new QTimer(this);
        m_busPollTimer->setInterval(BUS_POLL_INTERVAL_MS);

        connect(m_busPollTimer, &QTimer::timeout, this, &GST_Wrapper::pollBus);

    } // if (!m_busPollTimer)
    m_busPollTimer->start();

    gst_element_set_state(m_mpegTsPipeline, GST_STATE_PLAYING);
    m_IsActive_MPEG_TS = true;

    // Pump переливает кольцо←encode в appsrc транспорта; стартуем до запуска
    // транспорта — пока appsrc нет, pump простаивает, данные копятся в кольце
    // (drop-oldest), encode при этом полностью свободен от backpressure.
    startPumpThread();

    // Start transport pipeline (SRT/RTP via appsrc ← pump ← ring)
    startTransportPipeline();

    // Пайп поднят с РЕАЛЬНОЙ TV-камерой → разрешаем Main_app подключить EV9500M.
    // В videotestsrc-fallback (m_tvUseTestSrc) сигнал не идёт: реальной камеры нет.
    if (m_tvEnInPipeline && !m_tvUseTestSrc)
        emit sig_RealTvCameraUp();
} // GST_Wrapper::gst_Start_MPEG_TS_Pipeline

void GST_Wrapper::gst_StartTVPipeline()
{
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return; // на выходе конвейеры не поднимаем
    m_pipelineTV = run_GST_TVStream(sm->settings_store.tv_cam_video_device,
                                    sm->settings_store.ip_host, 5000);
    if (!m_pipelineTV)
    {
        g_printerr("Failed to create TV pipeline\n");
        return;
    } // if (!m_pipelineTV)
    gst_element_set_state(m_pipelineTV, GST_STATE_PLAYING);
    m_IsActiveTV = true;
} // GST_Wrapper::gst_StartTVPipeline

void GST_Wrapper::gst_StartIRPipeline()
{
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return; // на выходе конвейеры не поднимаем
    m_pipelineIR = run_GST_IRStreamISP(sm->settings_store.ir_cam_isp_video_device,
                                       sm->settings_store.ip_host, 5008);
    if (!m_pipelineIR)
    {
        g_printerr("Failed to create IR pipeline\n");
        return;
    } // if (!m_pipelineIR)
    gst_element_set_state(m_pipelineIR, GST_STATE_PLAYING);
    m_IsActiveIR = true;
} // GST_Wrapper::gst_StartIRPipeline

void GST_Wrapper::stopPipeline(GstElement *pipeline, gint timeout_ms)
{
    if (!pipeline)
        return;

    gst_element_set_state(pipeline, GST_STATE_NULL);
    // Ждём переход конечным таймаутом: без него get/set_state на зависшем v4l2-источнике
    // может стоять вечно и сорвать всё завершение процесса.
    const GstStateChangeReturn ret =
        gst_element_get_state(pipeline, nullptr, nullptr, timeout_ms * GST_MSECOND);

    if (ret == GST_STATE_CHANGE_ASYNC || ret == GST_STATE_CHANGE_FAILURE)
    {
        qWarning() << "[GST] pipeline still busy after" << timeout_ms << "ms";
        // В режиме завершения объект НЕ освобождаем: unref «под» незавершённым
        // streaming-переходом даёт зависание/крах на выходе (утечка на уходящий процесс безопаснее).
        if (!m_shutdownMode.load(std::memory_order_relaxed))
            gst_object_unref(pipeline);
        return;
    } // if (ret == GST_STATE_CHANGE_ASYNC || ret == GST_STATE_CHANGE_FAILURE)

    gst_object_unref(pipeline);

} // GST_Wrapper::stopPipeline

void GST_Wrapper::restartIRPipeline()
{
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return;
    if (m_pipelineIR)
    {
        stopPipeline(m_pipelineIR);
        m_pipelineIR = nullptr;
        m_IsActiveIR = false;
    } // if (m_pipelineIR)
    QTimer::singleShot(100, this, &GST_Wrapper::gst_StartIRPipeline);
} // GST_Wrapper::restartIRPipeline

void GST_Wrapper::restartTVPipeline()
{
    if (m_shutdownMode.load(std::memory_order_relaxed))
        return;
    if (m_pipelineTV)
    {
        stopPipeline(m_pipelineTV);
        m_pipelineTV = nullptr;
    } // if (m_pipelineTV)
    QTimer::singleShot(100, this, &GST_Wrapper::gst_StartTVPipeline);
} // GST_Wrapper::restartTVPipeline

void GST_Wrapper::proc_RestartIRPipeline()
{
    restartIRPipeline();
} // GST_Wrapper::proc_RestartIRPipeline

void GST_Wrapper::proc_RestartTVPipeline()
{
    restartTVPipeline();
} // GST_Wrapper::proc_RestartTVPipeline

void GST_Wrapper::stopMpegTsPipeline()
{
    m_IsActive_MPEG_TS = false;
    if (m_busPollTimer)
        m_busPollTimer->stop();
    if (m_mpegTsWatchdog)
        m_mpegTsWatchdog->stop();

    if (m_trackerThread)
    {
        TrackerCmd cmd;
        cmd.type = TrackerCmd::STOP;
        m_trackerThread->submitCmd(cmd);
        m_trackerThread->setActiveChannel(-1);
    } // if (m_trackerThread)

    m_telemSrc = nullptr;
    m_tvEnInPipeline = false;
    m_irEnInPipeline = false;
    m_tvUseTestSrc = false;
    m_irUseTestSrc = false;
    m_activeTrackerChannel = -1;
    m_chanEnabled[TV_CH] = false;
    m_chanEnabled[IR_CH] = false;

    stopTransportPipeline(); // гасит appsrc → освобождает возможный блокированный push
    stopPumpThread();        // join pump только после нейтрализации downstream
    m_tsRing.clear();

    if (m_mpegTsPipeline)
    {
        forceStopPipeline(m_mpegTsPipeline, 3000);
        gst_object_unref(m_mpegTsPipeline);
        m_mpegTsPipeline = nullptr;
        serviceIspCtrl("stop");
    } // if (m_mpegTsPipeline)

} // GST_Wrapper::stopMpegTsPipeline

void GST_Wrapper::restartMpegTsPipeline()
{
    if (!m_IsActive_MPEG_TS)
        return;

    // Stop encode ONLY — pipe + transport pipeline stay alive
    if (m_mpegTsWatchdog)
        m_mpegTsWatchdog->stop();

    if (m_trackerThread)
    {
        TrackerCmd cmd;
        cmd.type = TrackerCmd::STOP;
        m_trackerThread->submitCmd(cmd);
        m_trackerThread->setActiveChannel(-1);
    } // if (m_trackerThread)

    m_telemSrc = nullptr;
    m_tvEnInPipeline = false;
    m_irEnInPipeline = false;
    m_tvUseTestSrc = false;
    m_irUseTestSrc = false;
    m_activeTrackerChannel = -1;
    m_chanEnabled[TV_CH] = false;
    m_chanEnabled[IR_CH] = false;

    if (m_mpegTsPipeline)
    {
        forceStopPipeline(m_mpegTsPipeline, 3000);
        gst_object_unref(m_mpegTsPipeline);
        m_mpegTsPipeline = nullptr;
        serviceIspCtrl("stop");
    } // if (m_mpegTsPipeline)

    QTimer::singleShot(1000, this, [this]()
                       {
                           if (m_IsActive_MPEG_TS)
                               gst_Start_MPEG_TS_Pipeline();
                       }); // lambda [this] ()
} // GST_Wrapper::restartMpegTsPipeline

void GST_Wrapper::proc_stopCapture()
{
    // Идемпотентно: сигнал sig_GST_StopCapture и proc_shutdownAndQuit могут прийти оба;
    // повторный проход дал бы двойной gst_object_unref() → abort на завершении.
    bool expected = false;
    if (!m_captureStopped.compare_exchange_strong(expected, true))
        return;

    const gint budget = m_shutdownMode.load(std::memory_order_relaxed) ? 1500 : 3000;
    g_printerr("[GST] stopCapture begin (budget %d ms/pipeline)\n", budget);

    // Таймеры гасим первыми, чтобы watchdog/bus не дёргали уже остановленный конвейер
    // и не поставили в очередь новый рестарт.
    if (m_mpegTsWatchdog)
        m_mpegTsWatchdog->stop();
    if (m_busPollTimer)
        m_busPollTimer->stop();

    // Порядок важен: сначала источники (камеры), затем encode-конвейер (пишет в кольцо),
    // затем pump/транспорт. fakesink-drain гарантирует, что encode не зависает даже при
    // остановленном транспорте — блокировки на полном pipe больше нет.
    if (m_pipelineIR)
    {
        stopPipeline(m_pipelineIR, budget);
        m_pipelineIR = nullptr;
        m_IsActiveIR = false;
    } // if (m_pipelineIR)
    if (m_pipelineTV)
    {
        stopPipeline(m_pipelineTV, budget);
        m_pipelineTV = nullptr;
        m_IsActiveTV = false;
    } // if (m_pipelineTV)

    // Внутри: транспорт → pump(join) → кольцо → encode-конвейер (unref + обнуление).
    if (m_mpegTsPipeline || m_transportPipeline || m_pumpRun.load())
    {
        m_IsActive_MPEG_TS = false;
        stopMpegTsPipeline();
    } // if (m_mpegTsPipeline || m_transportPipeline || m_pumpRun.load())

    stopTrackerThread();
    g_printerr("[GST] stopCapture done\n");
} // GST_Wrapper::proc_stopCapture

void GST_Wrapper::proc_shutdownAndQuit()
{
    // Выполняется в worker-потоке: очистка уже поставлена в очередь раньше
    // (proc_stopCapture), ждём её фактического выполнения, затем выходим из event loop.
    m_shutdownMode.store(true, std::memory_order_release);

    // Останавливаемся здесь, если вызов по сигналу не дошёл (идемпотентно).
    proc_stopCapture();

    if (thread)
        thread->quit(); // → finished → proc_Thread_Finished
} // GST_Wrapper::proc_shutdownAndQuit

void GST_Wrapper::proc_Thread_Finished()
{
    if (m_busPollTimer)
    {
        m_busPollTimer->stop();
        delete m_busPollTimer;
        m_busPollTimer = nullptr;
    } // if (m_busPollTimer)
    if (m_mpegTsWatchdog)
    {
        m_mpegTsWatchdog->stop();
        delete m_mpegTsWatchdog;
        m_mpegTsWatchdog = nullptr;
    } // if (m_mpegTsWatchdog)

    // Транспорт → pump(join) → кольцо. Сначала гасим appsrc (освобождает возможный
    // блокированный push), затем join pump-потока, затем очищаем кольцо.
    stopTransportPipeline();
    stopPumpThread();
    m_tsRing.clear();

    stopTrackerThread();

    const gint budget = 1500;
    if (m_pipelineIR)
    {
        stopPipeline(m_pipelineIR, budget);
        m_pipelineIR = nullptr;
    } // if (m_pipelineIR)
    if (m_pipelineTV)
    {
        stopPipeline(m_pipelineTV, budget);
        m_pipelineTV = nullptr;
    } // if (m_pipelineTV)
    if (m_mpegTsPipeline)
    {
        stopPipeline(m_mpegTsPipeline, budget);
        m_mpegTsPipeline = nullptr;
    } // if (m_mpegTsPipeline)

    g_printerr("[GST] worker thread finished, all pipelines released\n");
} // GST_Wrapper::proc_Thread_Finished

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
GstElement *GST_Wrapper::run_GST_StreamMPEG_TS(
    QString tvDevice, QString irDevice, QString clientIP, qint32 portUDP)
{
    m_mpegTsParams = {tvDevice, irDevice, clientIP, portUDP};

    const QByteArray tvBA = tvDevice.toLocal8Bit();
    const QByteArray irBA = irDevice.toLocal8Bit();
    const auto &cfg = sm->settings_store;

    bool tvEn = cfg.tv_cam_en;
    bool irEn = cfg.ir_cam_en;
    const bool telEn = cfg.mpeg_ts_telemetry_en;

    m_tvUseTestSrc = false;
    m_irUseTestSrc = false;

    if (tvEn)
    {
        tvEn = v4l2CtlCheckWithRetries("TV", cfg.tv_cam_video_device,
                                       cfg.tv_cam_width, cfg.tv_cam_height, "RGB3",
                                       cfg.tv_cam_first_frame_timeout_ms,
                                       cfg.cam_frame_retry_timeout_ms,
                                       cfg.cam_frame_restart_count);
    } // if (tvEn)
    if (irEn)
    {
        irEn = v4l2CtlCheckWithRetries("IR", cfg.ir_cam_isp_video_device,
                                       cfg.ir_cam_width, cfg.ir_cam_height, "NV12",
                                       cfg.ir_cam_first_frame_timeout_ms,
                                       cfg.cam_frame_retry_timeout_ms,
                                       cfg.cam_frame_restart_count);
    } // if (irEn)
    if (!tvEn && !irEn)
    {
        g_printerr("[MPEG-TS] No cameras → videotestsrc fallback\n");
        tvEn = true;
        irEn = true;
        m_tvUseTestSrc = true;
        m_irUseTestSrc = true;
        // Синтетика: watchdog Main_app не должен завершать процесс по таймауту.
        m_syntheticVideo.store(true, std::memory_order_relaxed);
    } // if (!tvEn && !irEn)
    else
    {
        m_syntheticVideo.store(false, std::memory_order_relaxed);
    } // else

    m_tvEnInPipeline = tvEn;
    m_irEnInPipeline = irEn;

    if (!tvEn && !irEn && !telEn)
    {
        g_printerr("[MPEG-TS] Nothing enabled\n");
        return nullptr;
    } // if (!tvEn && !irEn && !telEn)

    const bool tvFlip = cfg.tv_cam_flip_180;
    const bool irFlip = cfg.ir_cam_flip_180;
    const bool useSrt = cfg.mpeg_ts_use_srt;
    const int srtTsAlignment = cfg.mpeg_ts_srt_ts_packet_count;

    const int muxAlignment = useSrt ? srtTsAlignment : 7;

    // ─── Build pipeline string ─────────────────────────────────────
    QString pipe;
    auto appendPipe = [&](const QString &s)
    {
        if (!pipe.isEmpty())
            pipe += " ";
        pipe += s;
    }; // lambda [&] (const QString &s)

    // TV branch
    if (tvEn)
    {
        QString tvSource = m_tvUseTestSrc
                               ? QString("videotestsrc name=tv_testsrc is-live=true do-timestamp=true pattern=0 ! "
                                         "video/x-raw,format=BGR,width=%1,height=%2,framerate=%3/1")
                                     .arg(cfg.tv_cam_width)
                                     .arg(cfg.tv_cam_height)
                                     .arg(cfg.tv_cam_target_fps)
                               : QString("v4l2src device=%1 do-timestamp=true ! "
                                         "video/x-raw,format=BGR,width=%2,height=%3")
                                     .arg(tvBA.data())
                                     .arg(cfg.tv_cam_width)
                                     .arg(cfg.tv_cam_height);

        appendPipe(tvSource + QString(
                                  " ! queue leaky=2 max-size-buffers=2 max-size-bytes=0 max-size-time=0 ! tee name=tv_tee"
                                  // Encoder branch → mux.sink_256
                                  " tv_tee. ! queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !" // ← LEAKY! protects tee
                                  " videoconvert ! video/x-raw,format=NV12 !"
                                  " videoflip name=tv_flip method=%1 !"
                                  " videorate ! video/x-raw,framerate=%2/1 !"
                                  " mpph265enc name=tv_enc ! h265parse config-interval=-1 !"
                                  " video/x-h265,stream-format=byte-stream,alignment=au !"
                                  " queue name=tv_enc_queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !" // ← LEAKY! protects tee
                                  " mux.sink_256"
                                  // Tracker branch → fakesink
                                  " tv_tee. ! queue leaky=2 max-size-buffers=1 max-size-bytes=0 max-size-time=0 !"
                                  " videoconvert ! video/x-raw,format=RGB,width=%3,height=%4 !"
                                  " videoflip name=tv_flip_track method=%1 !"
                                  " fakesink name=tv_trk_sink sync=false")
                                  .arg(tvFlip ? "rotate-180" : "none")
                                  .arg(cfg.tv_cam_target_fps)
                                  .arg(cfg.tv_cam_width)
                                  .arg(cfg.tv_cam_height));
    } // if (tvEn)

    // IR branch
    if (irEn)
    {
        QString irSource = m_irUseTestSrc
                               ? QString("videotestsrc name=ir_testsrc is-live=true do-timestamp=true pattern=0 ! "
                                         "video/x-raw,format=I420,width=%1,height=%2,framerate=%3/1")
                                     .arg(cfg.ir_cam_width)
                                     .arg(cfg.ir_cam_height)
                                     .arg(cfg.ir_cam_target_fps)
                               : QString("v4l2src device=%1 do-timestamp=true ! "
                                         "video/x-raw,width=%2,height=%3,framerate=%4/1")
                                     .arg(irBA.data())
                                     .arg(cfg.ir_cam_width)
                                     .arg(cfg.ir_cam_height)
                                     .arg(cfg.ir_cam_target_fps);

        appendPipe(irSource + QString(
                                  " ! queue leaky=2 max-size-buffers=2 max-size-bytes=0 max-size-time=0 !"
                                  " videorate ! video/x-raw,framerate=%1/1 ! tee name=ir_tee"
                                  // Encoder branch → mux.sink_257
                                  " ir_tee. ! queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !" // ← LEAKY! protects tee
                                  " videoflip name=ir_flip method=%2 !"
                                  " mpph265enc name=ir_enc ! h265parse config-interval=-1 !"
                                  " video/x-h265,stream-format=byte-stream,alignment=au !"
                                  " queue name=ir_enc_queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !" // ← LEAKY! protects tee
                                  " mux.sink_257"
                                  // Tracker branch → fakesink
                                  " ir_tee. ! queue leaky=2 max-size-buffers=1 max-size-bytes=0 max-size-time=0 !"
                                  " videoconvert ! video/x-raw,format=RGB,width=%3,height=%4 !"
                                  " videoflip name=ir_flip_track method=%2 !"
                                  " fakesink name=ir_trk_sink sync=false")
                                  .arg(cfg.ir_cam_target_fps)
                                  .arg(irFlip ? "rotate-180" : "none")
                                  .arg(cfg.ir_cam_width)
                                  .arg(cfg.ir_cam_height));
    } // if (irEn)

    // Telemetry
    if (telEn)
    {
        appendPipe(
            "appsrc name=telem_src is-live=true format=time do-timestamp=true "
            "caps=\"meta/x-klv,parsed=true\" !"
            " queue leaky=2 max-size-buffers=4 max-size-bytes=0 max-size-time=0 !" // ← LEAKY! protects appsrc
            " mux.sink_512");
    } // if (telEn)

    // Mux → fakesink. Данные перехватываются pad-probe'ом (mpegtsBridgeProbe) на
    // sink-pad этого fakesink и копируются в кольцо TsRingBuffer; pump-поток переливает
    // их в appsrc транспорта. kernel-pipe/fdsink больше нет: encode НИКОГДА не ждёт
    // транспорт — при затылке дропаются старые целые TS-пакеты (без corrupt).
    appendPipe(QString(
                   "mpegtsmux name=mux alignment=%1 pcr-interval=40000 pat-interval=40000"
                   " pmt-interval=40000 !"
                   " fakesink name=mpeg_ts_bridge_sink sync=false async=false")
                   .arg(muxAlignment));

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
    if (err || !pipeline)
    {
        g_printerr("Pipeline parse error: %s\n", err ? err->message : "nullptr");
        if (err)
            g_error_free(err);
        if (pipeline)
            gst_object_unref(pipeline);
        return nullptr;
    } // if (err || !pipeline)

    // Encoder settings
    if (tvEn)
    {
        GstElement *enc = gst_bin_get_by_name(GST_BIN(pipeline), "tv_enc");
        if (enc)
        {
            const guint64 bps = static_cast<guint64>(cfg.tv_cam_enc_target_bps);
            g_object_set(enc, "bps", bps, "gop", static_cast<guint>(cfg.tv_cam_target_fps),
                         "bps-max", static_cast<guint64>(bps * 1.2),
                         "bps-min", static_cast<guint64>(bps * 0.8), nullptr);
            gst_object_unref(enc);
        } // if (enc)
    } // if (tvEn)
    if (irEn)
    {
        GstElement *enc = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc");
        if (enc)
        {
            const guint64 bps = static_cast<guint64>(cfg.ir_cam_enc_target_bps);
            g_object_set(enc, "bps", bps, "gop", static_cast<guint>(cfg.ir_cam_target_fps),
                         "bps-max", static_cast<guint64>(bps * 1.2),
                         "bps-min", static_cast<guint64>(bps * 0.8), nullptr);
            gst_object_unref(enc);
        } // if (enc)
    } // if (irEn)

    // Telemetry appsrc
    if (telEn)
    {
        m_telemSrc = gst_bin_get_by_name(GST_BIN(pipeline), "telem_src");
        if (!m_telemSrc)
            g_printerr("WARNING: telem_src not found\n");
    } // if (telEn)

    // Tracker + probes
    if (!m_trackerThread)
    {
        m_trackerThread = new TrackerThread(&m_frameSlot, this);
        m_trackerThread->start();
    } // if (!m_trackerThread)
    m_iniPath = (QCoreApplication::applicationDirPath() + "/tracking.ini").toStdString();

    if (tvEn)
    {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "tv_trk_sink");
        if (sink)
        {
            GstPad *p = gst_element_get_static_pad(sink, "sink");
            if (p)
            {
                gst_pad_add_probe(p, (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST), &trackerFrameProbe, this, nullptr);
                gst_object_unref(p);
            } // if (p)
            gst_object_unref(sink);
        } // if (sink)
    } // if (tvEn)
    if (irEn)
    {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "ir_trk_sink");
        if (sink)
        {
            GstPad *p = gst_element_get_static_pad(sink, "sink");
            if (p)
            {
                gst_pad_add_probe(p, (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST), &trackerFrameProbe, this, nullptr);
                gst_object_unref(p);
            } // if (p)
            gst_object_unref(sink);
        } // if (sink)
    } // if (irEn)

    TrackerCmd tcmd;
    tcmd.type = TrackerCmd::REINIT;
    tcmd.iniPath = m_iniPath;
    m_trackerThread->submitCmd(tcmd);
    enableChan(tvEn ? TV_CH : IR_CH);

    // Camera watchdog probes (on encoder queues BEFORE mux — always see frames)
    m_tvLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    m_irLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());

    if (tvEn)
    {
        GstElement *q = gst_bin_get_by_name(GST_BIN(pipeline), "tv_enc_queue");
        if (q)
        {
            GstPad *p = gst_element_get_static_pad(q, "src");
            if (p)
            {
                gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, &tvFrameProbe, this, nullptr);
                gst_object_unref(p);
            } // if (p)
            gst_object_unref(q);
        } // if (q)
    } // if (tvEn)
    if (irEn)
    {
        GstElement *q = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc_queue");
        if (q)
        {
            GstPad *p = gst_element_get_static_pad(q, "src");
            if (p)
            {
                gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, &irFrameProbe, this, nullptr);
                gst_object_unref(p);
            } // if (p)
            gst_object_unref(q);
        } // if (q)
    } // if (irEn)

    // ─── Bridge: кольцо TS + pad-probe на sink-pad fakesink ──────────
    // Probe пишется в то же кольцо, из которого pump кормит appsrc транспорта.
    initTsBridge();
    GstElement *bridge_sink = gst_bin_get_by_name(GST_BIN(pipeline), "mpeg_ts_bridge_sink");
    if (bridge_sink)
    {
        GstPad *snk = gst_element_get_static_pad(bridge_sink, "sink");
        if (snk)
        {
            gst_pad_add_probe(snk,
                              (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST),
                              &GST_Wrapper::mpegtsBridgeProbe, this, nullptr);
            gst_object_unref(snk);
        } // if (snk)
        else
        {
            g_printerr("[Encode] bridge_sink has no sink pad\n");
        } // else
        gst_object_unref(bridge_sink);
    } // if (bridge_sink)
    else
    {
        g_printerr("[Encode] mpeg_ts_bridge_sink not found\n");
    } // else

    // Диагностика mux: считаем буферы на его src-pad, чтобы по [Bridge]-логу видеть,
    // выдаёт ли mpegtsmux пакеты вообще.
    // ВАЖНО: mpegtsmux с alignment= выдаёт данные сгруппированным GstBufferList, а не
    // одиночными GstBuffer. Зонд обязан слушать ОБА типа (BUFFER | BUFFER_LIST) — иначе
    // на выходе mux он не сработает и покажет ложный mux_out_total=0 (при живом потоке).
    {
        GstElement *muxe = gst_bin_get_by_name(GST_BIN(pipeline), "mux");
        if (muxe)
        {
            GstPad *msrc = gst_element_get_static_pad(muxe, "src");
            if (msrc)
            {
                gst_pad_add_probe(msrc,
                                  (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST),
                                  &GST_Wrapper::muxOutProbe, this, nullptr);
                gst_object_unref(msrc);
            } // if (msrc)
            // Входные счётчики по pad'ам: различают «кадры не доходят до mux» (in=0)
            // от «mux ждёт пустой live-pad телеметрии» (in256/257>0, klv=0).
            const struct
            {
                const char *pad;
                std::atomic<long long> *ctr;
            } // struct
            inProbes[] = {
                {"sink_256", &m_muxIn256},
                {"sink_257", &m_muxIn257},
                {"sink_512", &m_muxInKlv},
            };
            for (const auto &ip : inProbes)
            {
                GstPad *sp = gst_element_get_static_pad(muxe, ip.pad);
                if (sp)
                {
                    gst_pad_add_probe(sp, GST_PAD_PROBE_TYPE_BUFFER,
                                      &GST_Wrapper::muxSinkCountProbe, ip.ctr, nullptr);
                    gst_object_unref(sp);
                } // if (sp)
                else
                {
                    g_printerr("[Encode] mux pad %s not found for in-probe\n", ip.pad);
                } // else
            } // for (const auto &ip : inProbes)
            gst_object_unref(muxe);
        } // if (muxe)
        else
        {
            g_printerr("[Encode] mux element not found for diag probe\n");
        } // else
    } // block

    // Watchdog timer
    m_mpegTsPipeline = pipeline;
    if (tvEn || irEn)
    {
        if (!m_mpegTsWatchdog)
        {
            m_mpegTsWatchdog = new QTimer(this);
            m_mpegTsWatchdog->setInterval(WATCHDOG_INTERVAL_MS);

            connect(m_mpegTsWatchdog, &QTimer::timeout, this, &GST_Wrapper::onMpegTsWatchdogTimeout);

        } // if (!m_mpegTsWatchdog)
        m_mpegTsWatchdog->start();
    } // if (tvEn || irEn)

    return pipeline;
} // GST_Wrapper::run_GST_StreamMPEG_TS

// ═══════════════════════════════════════════════════════════════════
//  TELEMETRY
// ═══════════════════════════════════════════════════════════════════
bool GST_Wrapper::pushTelemetry(const void *data, gsize len, GstClockTime)
{
    if (!m_telemSrc || !m_mpegTsPipeline || !m_IsActive_MPEG_TS)
        return false;

    GstState state, pending;
    gst_element_get_state(m_mpegTsPipeline, &state, &pending, GST_CLOCK_TIME_NONE);
    if (state < GST_STATE_PAUSED)
        return false;

    GstClock *clock = gst_element_get_clock(m_mpegTsPipeline);
    GstClockTime base_time = gst_element_get_base_time(m_mpegTsPipeline);
    if (!clock || base_time == GST_CLOCK_TIME_NONE)
    {
        if (clock)
            gst_object_unref(clock);
        return true;
    } // if (!clock || base_time == GST_CLOCK_TIME_NONE)

    GstClockTime pts = gst_clock_get_time(clock) - base_time;
    gst_object_unref(clock);
    if (pts > (G_MAXUINT64 >> 1))
        return true;

    GstBuffer *buf = gst_buffer_new_allocate(nullptr, len, nullptr);
    if (!buf)
        return false;
    gst_buffer_fill(buf, 0, data, len);
    GST_BUFFER_PTS(buf) = pts;
    GST_BUFFER_DTS(buf) = pts;
    GST_BUFFER_DURATION(buf) = GST_SECOND / 10;

    GstFlowReturn ret;
    g_signal_emit_by_name(m_telemSrc, "push-buffer", buf, &ret);
    gst_buffer_unref(buf);
    return (ret == GST_FLOW_OK);
} // GST_Wrapper::pushTelemetry

void GST_Wrapper::proc_MPEG_TS_Telemetry(QByteArray datagram)
{
    if (datagram.isEmpty() || !m_IsActive_MPEG_TS)
        return;
    pushTelemetry(datagram.constData(), static_cast<gsize>(datagram.size()), GST_CLOCK_TIME_NONE);
} // GST_Wrapper::proc_MPEG_TS_Telemetry

// ═══════════════════════════════════════════════════════════════════
//  run_GST_TVStream (non-MPEGTS standalone mode)
// ═══════════════════════════════════════════════════════════════════
GstElement *GST_Wrapper::run_GST_TVStream(QString videoDevice, QString clientIP, qint32 portUDP)
{
    // ... same as before (unchanged) ...
    GstElement *pipeline = gst_pipeline_new("video-pipeline");
    GstElement *src = gst_element_factory_make("v4l2src", "source");
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement *queue = gst_element_factory_make("queue", "queue");
    GstElement *tee = gst_element_factory_make("tee", "tv_tee");
    GstElement *videorate = gst_element_factory_make("videorate", "rate");
    GstElement *capsfilter1 = gst_element_factory_make("capsfilter", "caps1");
    GstElement *encoder = gst_element_factory_make("mpph265enc", "encoder");
    GstElement *rtppay = gst_element_factory_make("rtph265pay", "rtppay");
    GstElement *sink = gst_element_factory_make("udpsink", "sink");
    GstElement *queue2 = gst_element_factory_make("queue", "tv_trk_queue");
    GstElement *trk_sink = gst_element_factory_make("fakesink", "tv_trk_sink");

    if (!pipeline || !src || !capsfilter || !queue || !tee || !videorate || !capsfilter1 || !encoder || !rtppay || !sink || !queue2 || !trk_sink)
    {
        g_printerr("Failed to create TV elements\n");
        return nullptr;
    } // if (!pipeline || !src || !capsfilter || !queue || !tee || !videorate || !capsfilter1 || !encoder || !rtppay || !sink ...

    GstCaps *caps = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, sm->settings_store.tv_cam_width,
                                        "height", G_TYPE_INT, sm->settings_store.tv_cam_height,
                                        "format", G_TYPE_STRING, "BGR", nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr);
    gst_caps_unref(caps);

    auto devBA = QByteArray(videoDevice.toLocal8Bit());
    g_object_set(src, "device", devBA.constData(), "min-buffers", 16, nullptr);
    g_object_set(queue, "leaky", 2, nullptr);
    g_object_set(queue2, "leaky", 2, nullptr);

    GstCaps *caps1 = gst_caps_new_simple("video/x-raw", "framerate", GST_TYPE_FRACTION, sm->settings_store.tv_cam_target_fps, 1, nullptr);
    g_object_set(capsfilter1, "caps", caps1, nullptr);
    gst_caps_unref(caps1);

    g_object_set(encoder, "bps", sm->settings_store.tv_cam_enc_target_bps, "gop", sm->settings_store.tv_cam_target_fps,
                 "bps-max", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 1.2),
                 "bps-min", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 0.8), nullptr);
    g_object_set(rtppay, "config-interval", 1, "pt", 96, "aggregate-mode", 0, nullptr);

    auto ipBA = QByteArray(clientIP.toLocal8Bit());
    g_object_set(sink, "host", ipBA.constData(), "port", portUDP, "sync", true, "async", false, nullptr);

    gst_bin_add_many(GST_BIN(pipeline), src, capsfilter, queue, tee, videorate, capsfilter1, encoder, rtppay, sink, queue2, trk_sink, nullptr);
    if (!gst_element_link_many(src, capsfilter, queue, tee, videorate, capsfilter1, encoder, rtppay, sink, nullptr))
    {
        g_printerr("TV encoder link failed\n");
        gst_object_unref(pipeline);
        return nullptr;
    } // if (!gst_element_link_many(src, capsfilter, queue, tee, videorate, capsfilter1, encoder, rtppay, sink, nullptr))

    GstPad *ts = gst_element_get_request_pad(tee, "src_%u");
    GstPad *qs = gst_element_get_static_pad(queue2, "sink");
    if (gst_pad_link(ts, qs) != GST_PAD_LINK_OK)
    {
        gst_object_unref(ts);
        gst_object_unref(qs);
        gst_object_unref(pipeline);
        return nullptr;
    } // if (gst_pad_link(ts, qs) != GST_PAD_LINK_OK)
    gst_object_unref(qs);
    if (!gst_element_link(queue2, trk_sink))
    {
        gst_object_unref(ts);
        gst_object_unref(pipeline);
        return nullptr;
    } // if (!gst_element_link(queue2, trk_sink))
    gst_object_unref(ts);

    GstPad *sp = gst_element_get_static_pad(trk_sink, "sink");
    if (sp)
    {
        gst_pad_add_probe(sp, GST_PAD_PROBE_TYPE_BUFFER, &trackerFrameProbe, this, nullptr);
        gst_object_unref(sp);
    } // if (sp)

    GstPad *qp = gst_element_get_static_pad(queue, "src");
    if (qp)
    {
        gst_pad_add_probe(qp, GST_PAD_PROBE_TYPE_BUFFER, &tvFrameProbe, this, nullptr);
        gst_object_unref(qp);
    } // if (qp)

    if (!m_trackerThread)
    {
        m_trackerThread = new TrackerThread(&m_frameSlot, this);
        m_trackerThread->start();
    } // if (!m_trackerThread)
    m_iniPath = (QCoreApplication::applicationDirPath() + "/tracking.ini").toStdString();
    TrackerCmd tc;
    tc.type = TrackerCmd::REINIT;
    tc.iniPath = m_iniPath;
    m_trackerThread->submitCmd(tc);
    m_activeTrackerChannel = TV_CH;
    m_trackerThread->setActiveChannel(TV_CH);

    return pipeline;
} // GST_Wrapper::run_GST_TVStream

// ═══════════════════════════════════════════════════════════════════
//  run_GST_IRStreamISP (non-MPEGTS standalone mode)
// ═══════════════════════════════════════════════════════════════════
GstElement *GST_Wrapper::run_GST_IRStreamISP(QString videoDevice, QString clientIP, qint32 portUDP)
{
    GstElement *pipeline = gst_pipeline_new("ir-pipeline");
    GstElement *src = gst_element_factory_make("v4l2src", "source");
    GstElement *queue = gst_element_factory_make("queue", "queue");
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement *videorate = gst_element_factory_make("videorate", "rate");
    GstElement *capsfilter1 = gst_element_factory_make("capsfilter", "caps1");
    GstElement *g16 = gst_element_factory_make("gray16togray8", "scaler");
    GstElement *capsfilter2 = gst_element_factory_make("capsfilter", "caps2");
    GstElement *convert2 = gst_element_factory_make("videoconvert", "convert2");
    GstElement *encoder = gst_element_factory_make("mpph265enc", "encoder");
    GstElement *rtppay = gst_element_factory_make("rtph265pay", "payloader");
    GstElement *sink = gst_element_factory_make("udpsink", "sink");

    GstElement *tee = gst_element_factory_make("tee", "ir_tee");
    GstElement *queue2 = gst_element_factory_make("queue", "ir_trk_queue");
    GstElement *trk_convert = gst_element_factory_make("videoconvert", "ir_trk_convert");
    GstElement *trk_caps = gst_element_factory_make("capsfilter", "ir_trk_caps");
    GstElement *trk_sink = gst_element_factory_make("fakesink", "ir_trk_sink");

    if (!pipeline || !src || !queue || !capsfilter || !videorate || !capsfilter1 || !g16 || !capsfilter2 || !convert2 || !encoder || !rtppay || !sink || !tee || !queue2 || !trk_convert || !trk_caps || !trk_sink)
    {
        g_printerr("Failed to create IR elements\n");
        return nullptr;
    } // if (!pipeline || !src || !queue || !capsfilter || !videorate || !capsfilter1 || !g16 || !capsfilter2 || !convert2 || ...

    GstCaps *caps = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                        "height", G_TYPE_INT, sm->settings_store.ir_cam_height, nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr);
    gst_caps_unref(caps);

    auto devBA = QByteArray(videoDevice.toLocal8Bit());
    g_object_set(src, "device", devBA.constData(), "do-timestamp", TRUE, "min-buffers", 7, nullptr);
    g_object_set(queue, "leaky", 2, nullptr);

    GstCaps *caps1 = gst_caps_new_simple("video/x-raw", "framerate", GST_TYPE_FRACTION, sm->settings_store.ir_cam_target_fps, 1, nullptr);
    g_object_set(capsfilter1, "caps", caps1, nullptr);
    gst_caps_unref(caps1);

    GstCaps *caps2 = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "GRAY8", "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                         "height", G_TYPE_INT, sm->settings_store.ir_cam_height, "framerate", GST_TYPE_FRACTION, sm->settings_store.ir_cam_target_fps, 1, nullptr);
    g_object_set(capsfilter2, "caps", caps2, nullptr);
    gst_caps_unref(caps2);

    GstCaps *trkOutCaps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.ir_cam_height, nullptr);
    g_object_set(trk_caps, "caps", trkOutCaps, nullptr);
    gst_caps_unref(trkOutCaps);

    g_object_set(encoder, "bps", sm->settings_store.ir_cam_enc_target_bps, "gop", sm->settings_store.ir_cam_target_fps,
                 "bps-max", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 1.2), "bps-min", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 0.8), nullptr);
    g_object_set(rtppay, "config-interval", 1, "pt", 96, "aggregate-mode", 0, nullptr);

    auto ipBA = QByteArray(clientIP.toLocal8Bit());
    g_object_set(sink, "host", ipBA.constData(), "port", portUDP, "sync", true, "async", false, nullptr);

    gst_bin_add_many(GST_BIN(pipeline), src, capsfilter, queue, tee, videorate, capsfilter1, g16, capsfilter2, convert2, encoder, rtppay, sink, queue2, trk_convert, trk_caps, trk_sink, nullptr);
    if (!gst_element_link_many(src, capsfilter, queue, tee, videorate, capsfilter1, g16, capsfilter2, convert2, encoder, rtppay, sink, nullptr))
    {
        g_printerr("IR encoder link failed\n");
        gst_object_unref(pipeline);
        return nullptr;
    } // if (!gst_element_link_many(src, capsfilter, queue, tee, videorate, capsfilter1, g16, capsfilter2, convert2, encoder, ...

    GstPad *ts = gst_element_get_request_pad(tee, "src_%u");
    GstPad *qs = gst_element_get_static_pad(queue2, "sink");
    if (gst_pad_link(ts, qs) != GST_PAD_LINK_OK)
    {
        gst_object_unref(ts);
        gst_object_unref(qs);
        gst_object_unref(pipeline);
        return nullptr;
    } // if (gst_pad_link(ts, qs) != GST_PAD_LINK_OK)
    gst_object_unref(qs);
    if (!gst_element_link_many(queue2, trk_convert, trk_caps, trk_sink, nullptr))
    {
        gst_object_unref(ts);
        gst_object_unref(pipeline);
        return nullptr;
    } // if (!gst_element_link_many(queue2, trk_convert, trk_caps, trk_sink, nullptr))
    gst_object_unref(ts);

    GstPad *sp = gst_element_get_static_pad(trk_sink, "sink");
    if (sp)
    {
        gst_pad_add_probe(sp, (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST), &trackerFrameProbe, this, nullptr);
        gst_object_unref(sp);
    } // if (sp)

    GstPad *qp = gst_element_get_static_pad(queue, "src");
    if (qp)
    {
        gst_pad_add_probe(qp, (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST), &irFrameProbe, this, nullptr);
        gst_object_unref(qp);
    } // if (qp)

    return pipeline;
} // GST_Wrapper::run_GST_IRStreamISP
