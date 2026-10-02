#include "gst_wrapper.h"

#include <gst/gst.h>
#include <gst/tag/tag.h>
#include <gst/play/gstplay.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <gst/gstmessage.h>
#include <qdatetime.h>
#include "fpsmonitor.h"
#include "gstgoentrack.h"

#include "QTimer"

  static gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer ptr) {
    //  qDebug()<<"GST_MESSAGE_TYPE" << GST_MESSAGE_TYPE(msg);

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
       // QCoreApplication::quit();
        break;
    case GST_MESSAGE_ERROR: {
        gchar* debug;
        GError* error;
        gst_message_parse_error(msg, &error, &debug);
        g_printerr("ERROR: %s\n", error->message);
        g_error_free(error);
        g_free(debug);
      //  QCoreApplication::quit();
        break;
    }
    // case GST_MESSAGE_QOS:
    //   //<<   bus->object.name;
    //     gboolean live;
    //     guint64 running_time;
    //     guint64 stream_time;
    //     guint64 timestamp;
    //     guint64 duration;
    //    // GstFormat format;

    //     // void            gst_message_parse_qos           (GstMessage * message, gboolean * live, guint64 * running_time,
    //     //                            guint64 * stream_time, guint64 * timestamp, guint64 * duration);

    //     gst_message_parse_qos(msg, &live, &running_time, &stream_time, &timestamp, &duration);

    //   //  proportion = (gdouble)processed / (processed + dropped);
    //     qDebug()<<"Frame QOS"<< live<<running_time<<stream_time<<timestamp<<duration;
    // break;

    case GST_MESSAGE_STREAM_START:
        g_print("Stream started!!\n");
        // gst_pipe_cnt++;
        // if((gst_pipe_cnt==2 ) && (gst_wrapper_inst !=nullptr))
        //     emit gst_wrapper_inst->sig_STREAM_RDY();

    default:
        break;
    }
    return TRUE;
}



GST_Wrapper::GST_Wrapper(SettingsManager *sm, QObject *parent)
    : QObject(parent)
    , sm {sm}

{
     qDebug() << Q_FUNC_INFO<< "GST_Wrapper thread ";


    connect(&thread, &QThread::started, this, [this]{
        gst_PipelineInit();
        qDebug()<<"GST Pipe init done" ;


    }, Qt::DirectConnection);

    QObject::connect(&thread, &QThread::finished, this, &GST_Wrapper::proc_Thread_Finished);

    moveToThread(&thread);
    thread.start();
    gst_wrapper_inst = this;

}

void GST_Wrapper::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
    if(m_pipelineIR != nullptr)
        delete (m_pipelineIR);
    if(m_pipelineTV != nullptr)
        delete (m_pipelineTV);
    if(m_trackerTV != nullptr)
        delete(m_trackerTV);
    if(m_trackerIR != nullptr)
        delete(m_trackerIR);
    if(m_mpegTsWatchdog != nullptr)
        delete(m_mpegTsWatchdog);

}


GST_Wrapper::~GST_Wrapper()
{
    qDebug() << Q_FUNC_INFO;

    qDebug() <<"GST stream Stop waiting...";
    while(m_IsActiveIR) thread.usleep(100);
    while(m_IsActiveTV) thread.usleep(100);
    while(m_IsActive_MPEG_TS) thread.usleep(100);
   // thread.usleep(3000);
    qDebug() <<"GST stream stopped";
    thread.exit();
    thread.wait();
}

static void fpsMeasurementCallback(GstElement *element, guint fps, guint droprate,
                                   guint avgfps, gpointer user_data) {
    GST_Wrapper *controller = static_cast<GST_Wrapper*>(user_data);
    QMetaObject::invokeMethod(controller, "handleFpsMeasurement",
                              Qt::QueuedConnection,
                              Q_ARG(uint, fps),
                              Q_ARG(uint, droprate),
                              Q_ARG(uint, avgfps));
}


// void GST_Wrapper::handleFpsMeasurement(uint fps, uint droprate, uint avgfps) {
//     qDebug() << "Current FPS:" << fps << "Droprate:" << droprate << "Average FPS:" << avgfps;

//     if (fps < 60 * 0.9) {
//         qWarning() << "Frame rate dropped below threshold!";
//         // Take corrective action
//     }
// }


void gstCleaner(GstElement* pipeline)
{
    GstIterator *iter = gst_bin_iterate_elements(GST_BIN(pipeline));
    GValue item = G_VALUE_INIT;
    gboolean done = FALSE;
    GstElement *element ;
    while (!done) {
        switch (gst_iterator_next(iter, &item)) {
        case GST_ITERATOR_OK:
            element = GST_ELEMENT(g_value_get_object(&item));
            // Remove and unref each element
            gst_bin_remove(GST_BIN(pipeline), element);
            gst_object_unref(element);
            g_value_unset(&item);
            break;
        case GST_ITERATOR_DONE:
            done = TRUE;
            break;
        case GST_ITERATOR_RESYNC:
            gst_iterator_resync(iter);
            break;
        case GST_ITERATOR_ERROR:
            done = TRUE;
            break;
        }
    }
    gst_iterator_free(iter);
}
void safe_cleanup(GstElement *pipeline_ptr) {

    GstElement *pipeline = pipeline_ptr;

    // Остановить и дождаться завершения
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_element_get_state(pipeline, NULL, NULL, GST_SECOND); // ждём до 1 сек

    // Убрать обработчик bus
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    if (bus) {
        gst_bus_remove_signal_watch(bus);
        gst_object_unref(bus);
    }

    // Освободить пайплайн
    gst_object_unref(pipeline);
    pipeline_ptr = NULL;
}

void GST_Wrapper::stopPipeline(GstElement* pipeline) {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        //safe_cleanup(pipeline);
        gst_object_unref(pipeline);

       // pipeline = nullptr;
    }

}
void GST_Wrapper::proc_stopCapture()
{
    m_trackerIR = nullptr;
    m_trackerTV = nullptr;
    if(m_pipelineIR)
    {
        stopPipeline(m_pipelineIR);
        m_IsActiveIR = false;
    }
    if(m_pipelineTV)
    {
        stopPipeline(m_pipelineTV);
        m_IsActiveTV = false;
    }
    if(m_pipelineMPEG_TS)
    {
        m_IsActive_MPEG_TS = false;
        //stopPipeline(m_pipelineMPEG_TS);
        stopMpegTsPipeline();

    }

}

void GST_Wrapper::restartIRPipeline() {
    if(m_pipelineIR){

        stopPipeline(m_pipelineIR);
        m_IsActiveIR = false;

    }
    QTimer::singleShot(100, this, &GST_Wrapper::gst_StartIRPipeline); // Small delay before restart
}

void GST_Wrapper::restartTVPipeline() {
    if(m_pipelineTV){
        stopPipeline(m_pipelineTV);

        QTimer::singleShot(100, this, &GST_Wrapper::gst_StartTVPipeline); // Small delay before restart
    }
}

void GST_Wrapper::proc_RestartIRPipeline()
{
    restartIRPipeline();
}

void GST_Wrapper::proc_RestartTVPipeline()
{
    restartTVPipeline();
}

void GST_Wrapper::gst_StartIRPipeline()
{



 //m_pipelineIR =  run_GST_IRStream(sm->settings_store.ir_cam_video_device, sm->settings_store.ip_host, 5008);
    m_pipelineIR =  run_GST_IRStreamISP(sm->settings_store.ir_cam_isp_video_device, sm->settings_store.ip_host, 5008);


    if (!m_pipelineIR) {
        g_printerr("Failed to create IR pipeline\n");
        return;
    }

    // Setup bus monitoring for both pipelines
    auto setup_bus = [](GstElement* pipeline) {
        GstBus* bus = gst_element_get_bus(pipeline);
        gst_bus_add_watch(bus, (GstBusFunc)(bus_call), nullptr);
        gst_object_unref(bus);
    };

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipelineIR));
    gst_bus_add_watch(bus, (GstBusFunc)bus_call, this);


     setup_bus(m_pipelineIR);
    //   setup_bus(m_pipelineTV);

    // Start both pipeline
    GstStateChangeReturn ret1 = gst_element_set_state(m_pipelineIR, GST_STATE_PLAYING);

     m_IsActiveIR = true;

}
void GST_Wrapper::gst_Start_MPEG_TS_Pipeline()
{

    m_pipelineMPEG_TS =    run_GST_StreamMPEG_TS(sm->settings_store.tv_cam_video_device,sm->settings_store.ir_cam_isp_video_device, sm->settings_store.ip_host,sm->settings_store.mpeg_ts_port );

    if (!m_pipelineMPEG_TS) {
        g_printerr("Failed to create TV pipeline\n");
        return;
    }

    // Setup bus monitoring for both pipelines
    auto setup_bus = [](GstElement* pipeline) {
        GstBus* bus = gst_element_get_bus(pipeline);
        gst_bus_add_watch(bus, (GstBusFunc)(bus_call), nullptr);
        gst_object_unref(bus);
    };

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipelineMPEG_TS));
    gst_bus_add_watch(bus, (GstBusFunc)bus_call, this);

    setup_bus(m_pipelineMPEG_TS);


    // Start pipeline
    GstStateChangeReturn ret2 = gst_element_set_state(m_pipelineMPEG_TS, GST_STATE_PLAYING);
    m_IsActive_MPEG_TS = true;

}
void GST_Wrapper::gst_StartTVPipeline()
{


    m_pipelineTV =    run_GST_TVStream(sm->settings_store.tv_cam_video_device, sm->settings_store.ip_host, 5000);

    if (!m_pipelineTV) {
        g_printerr("Failed to create TV pipeline\n");
        return;
    }

    // Setup bus monitoring for both pipelines
    auto setup_bus = [](GstElement* pipeline) {
        GstBus* bus = gst_element_get_bus(pipeline);
        gst_bus_add_watch(bus, (GstBusFunc)(bus_call), nullptr);
        gst_object_unref(bus);
    };

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipelineTV));
    gst_bus_add_watch(bus, (GstBusFunc)bus_call, this);

    setup_bus(m_pipelineTV);


    // Start pipeline
    GstStateChangeReturn ret2 = gst_element_set_state(m_pipelineTV, GST_STATE_PLAYING);
    m_IsActiveTV = true;

}


void GST_Wrapper::gst_PipelineInit()
{
    if(sm->settings_store.mpeg_ts_en)
    {
        gst_Start_MPEG_TS_Pipeline();

    }else {
        connect(this, &GST_Wrapper::sig_RstIRPipeline, this, &GST_Wrapper::proc_RestartIRPipeline , Qt::QueuedConnection);
        connect(this, &GST_Wrapper::sig_RstTVPipeline, this, &GST_Wrapper::proc_RestartTVPipeline , Qt::QueuedConnection);





        gst_StartTVPipeline();
        gst_StartIRPipeline();
    }

    emit sig_STREAM_RDY();

}



static void onFpsUpdateTV(gdouble fps, gboolean active_stream, gpointer user_data) {
    GST_Wrapper *self = static_cast<GST_Wrapper*>(user_data);
    QMetaObject::invokeMethod(self, "handleFpsUpdateTV", Qt::QueuedConnection,
                                Q_ARG(double, fps),
                                Q_ARG(bool, active_stream));
}


static void onFpsUpdateIR(gdouble fps, gboolean active_stream, gpointer user_data) {
    GST_Wrapper *self = static_cast<GST_Wrapper*>(user_data);
    QMetaObject::invokeMethod(self, "handleFpsUpdateIR", Qt::QueuedConnection,
                                Q_ARG(double, fps),
                                Q_ARG(bool, active_stream));
}



void GST_Wrapper::handleFpsUpdateTV(double fps, bool active_stream) {

    if (fps < 0.1)
    {

        if(m_tv_wd<10)
        {
            m_tv_wd ++;
            qDebug() << "TV stream stalled. WatchDog value"  << m_tv_wd;
        }
        else
        {
            qDebug() << "TV stream cannot resume. Restart pipeline... ";
            emit sig_RstTVPipeline();
            m_tv_wd = 0;

        }
    }
    else
    {
        qDebug() << "Current TV FPS:" << fps<< "Stream active:" << active_stream;
        m_tv_wd = 0;
    }

  //  emit fpsUpdated(fps);
}

void GST_Wrapper::handleFpsUpdateIR(double fps, bool active_stream) {
    if (fps < 0.1)
    {

        if(m_ir_wd<10)
        {
            m_ir_wd ++;
            qDebug() << "IR stream stalled. WatchDog value"  << m_ir_wd;
        }
        else
        {
            qDebug() << "IR stream cannot resume. Restart pipeline... ";
            emit sig_RstIRPipeline();
            m_ir_wd = 0;

        }
    }
    else
    {
        qDebug() << "Current IR FPS:" << fps<< "Stream active:" << active_stream;;
        m_ir_wd = 0;
    }

    //  emit fpsUpdated(fps);
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
void GST_Wrapper::onMpegTsWatchdogTimeout()
{
    const qint64 now     = QDateTime::currentMSecsSinceEpoch();
    const bool   tvEn    = sm->settings_store.tv_cam_en;
    const bool   irEn    = sm->settings_store.ir_cam_en;

    const bool tvStale = tvEn && (now - m_tvLastFrameMs.load() > WATCHDOG_TIMEOUT_MS);
    const bool irStale = irEn && (now - m_irLastFrameMs.load() > WATCHDOG_TIMEOUT_MS);

    if (tvStale || irStale) {
        g_printerr("WATCHDOG: stale stream detected (tv=%d ir=%d) — restarting pipeline\n",
                   (int)tvStale, (int)irStale);
        restartMpegTsPipeline();
    }
}

void GST_Wrapper::stopMpegTsPipeline()
{
  //  std::lock_guard<std::mutex> lock(m_mutex);
    m_IsActive_MPEG_TS = false;
    // Сначала снимаем watch, чтобы новые сигналы не приходили
    if (m_mpegTsPipeline) {
        GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_mpegTsPipeline));
        if (bus) {
            gst_bus_remove_signal_watch(bus);
            gst_object_unref(bus);
        }
    }

    // Останавливаем watchdog
    if (m_mpegTsWatchdog)
        m_mpegTsWatchdog->stop();

    // Освобождаем ссылки на дочерние элементы
    if (m_telemSrc) {
        gst_object_unref(m_telemSrc);
        m_telemSrc = nullptr;
    }

    if (m_trackerTV) {
        gst_object_unref(m_trackerTV);
        m_trackerTV = nullptr;
    }
    if (m_trackerIR) {
        gst_object_unref(m_trackerIR);
        m_trackerIR = nullptr;
    }



    // Останавливаем и удаляем пайплайн
    if (m_mpegTsPipeline) {
        gst_element_set_state(m_mpegTsPipeline, GST_STATE_NULL);
        gst_element_get_state(m_mpegTsPipeline, nullptr, nullptr, GST_SECOND);
        gst_object_unref(m_mpegTsPipeline);
        m_mpegTsPipeline = nullptr;
    }
}

void GST_Wrapper::restartMpegTsPipeline()
{
    m_IsActive_MPEG_TS = false;
    // Watchdog остановим на время пересоздания, чтобы не войти повторно
    if (m_mpegTsWatchdog)
        m_mpegTsWatchdog->stop();
    stopMpegTsPipeline();

    // Небольшая пауза — даём ядру освободить устройства v4l2
    QThread::msleep(5000);

    gst_Start_MPEG_TS_Pipeline();

}

GstElement* GST_Wrapper::run_GST_StreamMPEG_TS(QString tvDevice,
                                               QString irDevice,
                                               QString clientIP,
                                               qint32  portUDP)
{
    // ── Сохраняем параметры для перезапуска ─────────────────────
    m_mpegTsParams = { tvDevice, irDevice, clientIP, portUDP };

    QByteArray ipBA = clientIP.toLocal8Bit();
    QByteArray tvBA = tvDevice.toLocal8Bit();
    QByteArray irBA = irDevice.toLocal8Bit();
    const bool tvEn      = sm->settings_store.tv_cam_en;
    const bool irEn      = sm->settings_store.ir_cam_en;
    const bool telEn     = sm->settings_store.mpeg_ts_telemetry_en;
    // ── НОВОЕ: флаги переворота ──────────────────────────────────
    const bool tvFlip    = sm->settings_store.tv_cam_flip_180;
    const bool irFlip    = sm->settings_store.ir_cam_flip_180;
    QString pipe;

    // ===================== TV =====================
    if (tvEn) {
        // videoflip вставляется после videoconvert перед энкодером.
        // method=rotate-180 при tvFlip=true, иначе method=none (identity).
        pipe += QString(
                    "v4l2src device=%1 do-timestamp=true ! "
                    "video/x-raw,format=BGR,width=%2,height=%3 ! "
                    "queue leaky=2 max-size-buffers=10 ! "
                    "tee name=tv_tee "

                    "tv_tee. ! queue leaky=2 max-size-buffers=10 ! "
                    "videoconvert ! video/x-raw,format=NV12 ! "
                    // ── НОВОЕ: videoflip для ветви энкодера ──────
                    "videoflip name=tv_flip method=%4 ! "
                    // ─────────────────────────────────────────────
                    "videorate ! video/x-raw,framerate=%5/1 ! "
                    "mpph265enc name=tv_enc ! "
                    "h265parse config-interval=-1 ! "
                    "video/x-h265,stream-format=byte-stream,alignment=au ! "
                    "queue ! mux.sink_256 "

                    "tv_tee. ! queue leaky=2 max-size-buffers=4 ! "
                    "videoconvert ! "
                    "video/x-raw,format=RGB,width=%2,height=%3 ! "
                    // ── НОВОЕ: videoflip для трекерной ветви ─────
                    "videoflip name=tv_flip_track method=%4 ! "
                    // ─────────────────────────────────────────────
                    "goentrack name=tv_tracker ! "
                    "fpsmonitor name=tv_fpsmon ! "
                    "fakesink name=tv_fakesink sync=false "
                    )
                    .arg(tvBA.data())                                    // %1
                    .arg(sm->settings_store.tv_cam_width)                // %2
                    .arg(sm->settings_store.tv_cam_height)               // %3
                    .arg(tvFlip ? "rotate-180" : "none")                 // %4 ── НОВОЕ
                    .arg(sm->settings_store.tv_cam_target_fps);          // %5
    }

    // ===================== IR =====================
    if (irEn) {
        pipe += QString(
                    "v4l2src device=%1 do-timestamp=true ! "
                    "video/x-raw,width=%2,height=%3,framerate=%4/1 ! "
                    "queue leaky=2 max-size-buffers=10 ! "
                    "videorate ! video/x-raw,framerate=%4/1 ! "
                    // ── НОВОЕ: videoflip для IR ───────────────────
                    "videoflip name=ir_flip method=%5 ! "
                    // ─────────────────────────────────────────────
                    "mpph265enc name=ir_enc ! "
                    "h265parse config-interval=-1 ! "
                    "video/x-h265,stream-format=byte-stream,alignment=au ! "
                    "queue name=ir_enc_queue ! mux.sink_257 "
                    ).arg(irBA.data())                                   // %1
                    .arg(sm->settings_store.ir_cam_width)                // %2
                    .arg(sm->settings_store.ir_cam_height)               // %3
                    .arg(sm->settings_store.ir_cam_target_fps)           // %4
                    .arg(irFlip ? "rotate-180" : "none");                // %5 ── НОВОЕ
    }

    // ===================== TELEMETRY =====================
    if (telEn) {
        pipe += QString(
            "appsrc name=telem_src is-live=true format=time do-timestamp=true "
            "caps=\"meta/x-klv,parsed=true\" ! "
            "queue leaky=2 max-size-buffers=4 ! "
            "mux.sink_512 "
            );
    }

    pipe += QString(
                "mpegtsmux name=mux "
                "alignment=7 "
                "pcr-interval=40000 "
                "pat-interval=40000 "
                "pmt-interval=40000 ! "
                "queue ! "
                "rtpmp2tpay pt=33 mtu=1328 ! "
                "udpsink host=%1 port=%2 sync=false async=false"
                ).arg(ipBA.data())
                .arg(portUDP);

    g_printerr("\nPIPELINE:\n%s\n\n", pipe.toUtf8().data());

    GError *err = nullptr;
    GstElement *pipeline = gst_parse_launch(pipe.toUtf8().data(), &err);
    if (!pipeline || err) {
        g_printerr("Pipeline error: %s\n", err ? err->message : "unknown");
        if (err) g_error_free(err);
        return nullptr;
    }

    // ===================== TV энкодер =====================
    if (tvEn) {
        GstElement *tvEnc = gst_bin_get_by_name(GST_BIN(pipeline), "tv_enc");
        if (tvEnc) {
            g_object_set(tvEnc,
                         "bps",     sm->settings_store.tv_cam_enc_target_bps,
                         "gop",     sm->settings_store.tv_cam_target_fps,
                         "bps-max", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 1.2),
                         "bps-min", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 0.8),
                         nullptr);
            gst_object_unref(tvEnc);
        } else {
            g_printerr("WARNING: tv_enc not found\n");
        }
    }

    // ===================== IR энкодер =====================
    if (irEn) {
        GstElement *irEnc = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc");
        if (irEnc) {
            g_object_set(irEnc,
                         "bps",     sm->settings_store.ir_cam_enc_target_bps,
                         "gop",     sm->settings_store.ir_cam_target_fps,
                         "bps-max", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 1.2),
                         "bps-min", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 0.8),
                         nullptr);
            gst_object_unref(irEnc);
        } else {
            g_printerr("WARNING: ir_enc not found\n");
        }
    }

    // ===================== appsrc =====================
    if (telEn) {
        m_telemSrc = gst_bin_get_by_name(GST_BIN(pipeline), "telem_src");
    }

    // ===================== Трекер TV =====================
    if (tvEn) {
        GstElement *tracker = gst_bin_get_by_name(GST_BIN(pipeline), "tv_tracker");
        if (tracker) {
            m_trackerTV = tracker;
            const char sTrackINIFilename[] = "tracking.ini";
            handle_set_cfg_filename(tracker, (char*)sTrackINIFilename);
            gst_object_unref(tracker);
        } else {
            g_printerr("WARNING: tv_tracker not found\n");
        }

        GstElement *fpsmon = gst_bin_get_by_name(GST_BIN(pipeline), "tv_fpsmon");
        if (fpsmon) {
            GstFpsMonitor *monitor = GST_FPS_MONITOR(fpsmon);
            monitor->callback  = &onFpsUpdateTV;
            monitor->user_data = this;
            monitor->interval  = 1000;
            gst_object_unref(fpsmon);
        } else {
            g_printerr("WARNING: tv_fpsmon not found\n");
        }
    }

    // ===================== Pad probes для watchdog =====================
    m_tvLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
    m_irLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());

    if (tvEn) {
        GstElement *tvFakesink = gst_bin_get_by_name(GST_BIN(pipeline), "tv_fakesink");
        if (tvFakesink) {
            GstPad *sinkPad = gst_element_get_static_pad(tvFakesink, "sink");
            if (sinkPad) {
                gst_pad_add_probe(sinkPad,
                                  GST_PAD_PROBE_TYPE_BUFFER,
                                  &GST_Wrapper::tvFrameProbe,
                                  this, nullptr);
                gst_object_unref(sinkPad);
            }
            gst_object_unref(tvFakesink);
        }
    }

    if (irEn) {
        GstElement *irQueue = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc_queue");
        if (irQueue) {
            GstPad *srcPad = gst_element_get_static_pad(irQueue, "src");
            if (srcPad) {
                gst_pad_add_probe(srcPad,
                                  GST_PAD_PROBE_TYPE_BUFFER,
                                  &GST_Wrapper::irFrameProbe,
                                  this, nullptr);
                gst_object_unref(srcPad);
            }
            gst_object_unref(irQueue);
        }
    }

    // ===================== Запуск watchdog =====================
    m_mpegTsPipeline = pipeline;

    if (!m_mpegTsWatchdog) {
        m_mpegTsWatchdog = new QTimer(this);
        m_mpegTsWatchdog->setInterval(WATCHDOG_INTERVAL_MS);
        connect(m_mpegTsWatchdog, &QTimer::timeout,
                this, &GST_Wrapper::onMpegTsWatchdogTimeout);
    }
    m_mpegTsWatchdog->start();

    g_printerr("PIPELINE STARTED (string mode)\n");
    return pipeline;
}

// GstElement* GST_Wrapper::run_GST_StreamMPEG_TS(QString tvDevice,
//                                                QString irDevice,
//                                                QString clientIP,
//                                                qint32  portUDP)
// {
//     // ── Сохраняем параметры для перезапуска ─────────────────────
//     m_mpegTsParams = { tvDevice, irDevice, clientIP, portUDP };

//     QByteArray ipBA = clientIP.toLocal8Bit();
//     QByteArray tvBA = tvDevice.toLocal8Bit();
//     QByteArray irBA = irDevice.toLocal8Bit();
//     const bool tvEn  = sm->settings_store.tv_cam_en;
//     const bool irEn  = sm->settings_store.ir_cam_en;
//     const bool telEn = sm->settings_store.mpeg_ts_telemetry_en;
//     QString pipe;

//     // ===================== TV =====================
//     if (tvEn) {
//         pipe += QString(
//                     "v4l2src device=%1 do-timestamp=true ! "
//                     "video/x-raw,format=BGR,width=%2,height=%3 ! "
//                     "queue leaky=2 max-size-buffers=10 ! "
//                     "tee name=tv_tee "

//                     "tv_tee. ! queue leaky=2 max-size-buffers=10 ! "
//                     "videoconvert ! video/x-raw,format=NV12 ! "
//                     "videorate ! video/x-raw,framerate=%4/1 ! "
//                     "mpph265enc name=tv_enc ! "
//                     "h265parse config-interval=-1 ! "
//                     "video/x-h265,stream-format=byte-stream,alignment=au ! "
//                     "queue ! mux.sink_256 "

//                     "tv_tee. ! queue leaky=2 max-size-buffers=4 ! "
//                     "videoconvert ! "
//                     "video/x-raw,format=RGB,width=%2,height=%3 ! "
//                     "goentrack name=tv_tracker ! "
//                     "fpsmonitor name=tv_fpsmon ! "
//                     "fakesink name=tv_fakesink sync=false "
//                     )
//                     .arg(tvBA.data())
//                     .arg(sm->settings_store.tv_cam_width)
//                     .arg(sm->settings_store.tv_cam_height)
//                     .arg(sm->settings_store.tv_cam_target_fps);
//     }

//     // ===================== IR =====================
//     if (irEn) {
//         pipe += QString(
//                     "v4l2src device=%1 do-timestamp=true ! "
//                     "video/x-raw,width=%2,height=%3,framerate=%4/1 ! "
//                     "queue leaky=2 max-size-buffers=10 ! "
//                     "videorate ! video/x-raw,framerate=%4/1 ! "
//                     "mpph265enc name=ir_enc ! "
//                     "h265parse config-interval=-1 ! "
//                     "video/x-h265,stream-format=byte-stream,alignment=au ! "
//                     "queue name=ir_enc_queue ! mux.sink_257 "   // имя для probe
//                     ).arg(irBA.data())
//                     .arg(sm->settings_store.ir_cam_width)
//                     .arg(sm->settings_store.ir_cam_height)
//                     .arg(sm->settings_store.ir_cam_target_fps);
//     }

//     // ===================== TELEMETRY =====================
//     if (telEn) {
//         pipe += QString(
//             "appsrc name=telem_src is-live=true format=time do-timestamp=true "
//             "caps=\"meta/x-klv,parsed=true\" ! "
//             "queue leaky=2 max-size-buffers=4 ! "
//             "mux.sink_512 "
//             );
//     }

//     pipe += QString(
//                 "mpegtsmux name=mux "
//                 "alignment=7 "
//                 "pcr-interval=40000 "
//                 "pat-interval=40000 "
//                 "pmt-interval=40000 ! "
//                 "queue ! "
//                 "rtpmp2tpay pt=33 mtu=1328 ! "
//                 "udpsink host=%1 port=%2 sync=false async=false"
//                 ).arg(ipBA.data())
//                 .arg(portUDP);

//     g_printerr("\nPIPELINE:\n%s\n\n", pipe.toUtf8().data());

//     GError *err = nullptr;
//     GstElement *pipeline = gst_parse_launch(pipe.toUtf8().data(), &err);
//     if (!pipeline || err) {
//         g_printerr("Pipeline error: %s\n", err ? err->message : "unknown");
//         if (err) g_error_free(err);
//         return nullptr;
//     }

//     // ===================== TV энкодер =====================
//     if (tvEn) {
//         GstElement *tvEnc = gst_bin_get_by_name(GST_BIN(pipeline), "tv_enc");
//         if (tvEnc) {
//             g_object_set(tvEnc,
//                          "bps",     sm->settings_store.tv_cam_enc_target_bps,
//                        //  "rc-mode", sm->settings_store.tv_cam_enc_mode.toUtf8().constData(),
//                          "gop",     sm->settings_store.tv_cam_target_fps,
//                          "bps-max", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 1.2),
//                          "bps-min", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 0.8),
//                          nullptr);
//             gst_object_unref(tvEnc);
//         } else {
//             g_printerr("WARNING: tv_enc not found\n");
//         }
//     }

//     // ===================== IR энкодер =====================
//     if (irEn) {
//         GstElement *irEnc = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc");
//         if (irEnc) {
//             g_object_set(irEnc,
//                          "bps",     sm->settings_store.ir_cam_enc_target_bps,
//                  //        "rc-mode", sm->settings_store.ir_cam_enc_mode.toUtf8().constData(),
//                          "gop",     sm->settings_store.ir_cam_target_fps,
//                          "bps-max", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 1.2),
//                          "bps-min", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 0.8),
//                          nullptr);
//             gst_object_unref(irEnc);
//         } else {
//             g_printerr("WARNING: ir_enc not found\n");
//         }
//     }

//     // ===================== appsrc =====================
//     if (telEn) {
//         m_telemSrc = gst_bin_get_by_name(GST_BIN(pipeline), "telem_src");
//     }

//     // ===================== Трекер TV =====================
//     if (tvEn) {
//         GstElement *tracker = gst_bin_get_by_name(GST_BIN(pipeline), "tv_tracker");
//         if (tracker) {
//             m_trackerTV = tracker;
//             const char sTrackINIFilename[] = "tracking.ini";
//             handle_set_cfg_filename(tracker, (char*)sTrackINIFilename);
//             gst_object_unref(tracker);
//         } else {
//             g_printerr("WARNING: tv_tracker not found\n");
//         }

//         GstElement *fpsmon = gst_bin_get_by_name(GST_BIN(pipeline), "tv_fpsmon");
//         if (fpsmon) {
//             GstFpsMonitor *monitor = GST_FPS_MONITOR(fpsmon);
//             monitor->callback  = &onFpsUpdateTV;
//             monitor->user_data = this;
//             monitor->interval  = 1000;
//             gst_object_unref(fpsmon);
//         } else {
//             g_printerr("WARNING: tv_fpsmon not found\n");
//         }
//     }

//     // ===================== Pad probes для watchdog =====================
//     // Сбрасываем счётчики перед установкой probes
//     m_tvLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
//     m_irLastFrameMs.store(QDateTime::currentMSecsSinceEpoch());

//     if (tvEn) {
//         // Probe на sink-пад fakesink: кадры трекерной ветви
//         GstElement *tvFakesink = gst_bin_get_by_name(GST_BIN(pipeline), "tv_fakesink");
//         if (tvFakesink) {
//             GstPad *sinkPad = gst_element_get_static_pad(tvFakesink, "sink");
//             if (sinkPad) {
//                 gst_pad_add_probe(sinkPad,
//                                   GST_PAD_PROBE_TYPE_BUFFER,
//                                   &GST_Wrapper::tvFrameProbe,
//                                   this, nullptr);
//                 gst_object_unref(sinkPad);
//             }
//             gst_object_unref(tvFakesink);
//         }
//     }

//     if (irEn) {
//         // Probe на src-пад ir_enc_queue → кадры идут в mux
//         GstElement *irQueue = gst_bin_get_by_name(GST_BIN(pipeline), "ir_enc_queue");
//         if (irQueue) {
//             GstPad *srcPad = gst_element_get_static_pad(irQueue, "src");
//             if (srcPad) {
//                 gst_pad_add_probe(srcPad,
//                                   GST_PAD_PROBE_TYPE_BUFFER,
//                                   &GST_Wrapper::irFrameProbe,
//                                   this, nullptr);
//                 gst_object_unref(srcPad);
//             }
//             gst_object_unref(irQueue);
//         }
//     }

//     // ===================== Запуск watchdog =====================
//     m_mpegTsPipeline = pipeline;

//     if (!m_mpegTsWatchdog) {
//         m_mpegTsWatchdog = new QTimer(this);
//         m_mpegTsWatchdog->setInterval(WATCHDOG_INTERVAL_MS);
//         connect(m_mpegTsWatchdog, &QTimer::timeout,
//                 this, &GST_Wrapper::onMpegTsWatchdogTimeout);
//     }
//     m_mpegTsWatchdog->start();

//     g_printerr("PIPELINE STARTED (string mode)\n");
//     return pipeline;
// }

bool GST_Wrapper::pushTelemetry(const void *data, gsize len, GstClockTime)
{
    if (!m_telemSrc || !m_pipelineMPEG_TS || !m_IsActive_MPEG_TS) return false;

    GstState state, pending;
    gst_element_get_state(m_pipelineMPEG_TS, &state, &pending, GST_CLOCK_TIME_NONE);
    if (state < GST_STATE_PAUSED) {
        // Пайплайн ещё не готов — пропускаем обработку или откладываем
        return false;
    }

    GstClock     *clock     = gst_element_get_clock(m_pipelineMPEG_TS);
    GstClockTime  base_time = gst_element_get_base_time(m_pipelineMPEG_TS);

    // Пайплайн ещё не синхронизировался — пропускаем (как в Python)
    if (!clock || base_time == GST_CLOCK_TIME_NONE) {
        if (clock) gst_object_unref(clock);
        return true;
    }

    GstClockTime pts = gst_clock_get_time(clock) - base_time;
    gst_object_unref(clock);

    if (pts > (G_MAXUINT64 >> 1)) return true; // underflow guard

    GstBuffer *buf = gst_buffer_new_allocate(nullptr, len, nullptr);
    if (!buf) return false;

    gst_buffer_fill(buf, 0, data, len);

    GST_BUFFER_PTS(buf)      = pts;
    GST_BUFFER_DTS(buf)      = GST_CLOCK_TIME_NONE;
    GST_BUFFER_DURATION(buf) = GST_SECOND / 10; // 100 мс
    GST_BUFFER_OFFSET(buf)   = GST_BUFFER_OFFSET_NONE;
    GST_BUFFER_FLAG_UNSET(buf, GST_BUFFER_FLAG_DELTA_UNIT);

    GstFlowReturn ret;
    g_signal_emit_by_name(m_telemSrc, "push-buffer", buf, &ret);
    gst_buffer_unref(buf);

    if (ret == GST_FLOW_FLUSHING) return false;
    if (ret != GST_FLOW_OK)
        g_printerr("[KLV] push-buffer error: %d\n", ret);

    return ret == GST_FLOW_OK;
}


void GST_Wrapper::proc_MPEG_TS_Telemetry(QByteArray datagram)
{
    if ( datagram.isEmpty()&& !m_IsActive_MPEG_TS) return;
         pushTelemetry(
        static_cast<const void*>(datagram.constData()),
        static_cast<gsize>(datagram.size()),
        GST_CLOCK_TIME_NONE
        );
}

GstElement *GST_Wrapper::run_GST_TVStream(QString videoDevice, QString clientIP, qint32 portUDP)
{


    // Create elements
    GstElement* pipeline = gst_pipeline_new("video-pipeline");
    GstElement* src = gst_element_factory_make("v4l2src", "source");
    GstElement* testsrc = gst_element_factory_make("videotestsrc", "source");
    GstElement* fpsmon = gst_element_factory_make("fpsmonitor", "fpsmon");
    GstElement* videoconvert = gst_element_factory_make("videoconvert", "conv");

    GstElement* capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement* identity = gst_element_factory_make("identity", "ident");
    GstElement* identity2 = gst_element_factory_make("identity", "ident2");

    GstElement* track = gst_element_factory_make("goentrack", "scaler");

    GstElement* convert1 = gst_element_factory_make("videoconvert", "convert1");


    GstElement* capsfiltertrack0= gst_element_factory_make("capsfilter", "capstrack0");
    GstElement* capsfiltertrack1= gst_element_factory_make("capsfilter", "capstrack1");

    GstElement* pipe = gst_element_factory_make("tee", "pipe");
    GstElement* queue = gst_element_factory_make("queue", "queue");
    GstElement* queue2 = gst_element_factory_make("queue", "queue2");

    GstElement* videorate = gst_element_factory_make("videorate", "rate");
    GstElement* capsfilter1 = gst_element_factory_make("capsfilter", "caps1");

    // GstElement* capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    GstElement* encoder = gst_element_factory_make("mpph265enc", "encoder");
    GstElement* rtppay = gst_element_factory_make("rtph265pay", "rtppay");
    GstElement* sink = gst_element_factory_make("udpsink", "sink");
    GstElement* lb = gst_element_factory_make("v4l2sink", "lbsource");

    GstElement* fakesink = gst_element_factory_make("fakesink", "fsink");


    if (!pipeline || !src || !track  || ! fpsmon   || ! identity ||! identity2 ||  ! videoconvert || !capsfilter || !queue || ! pipe || !videorate || !capsfilter1 || !encoder || !rtppay || !sink || !lb || !fakesink) {
        g_printerr("Failed to create elements\n");
        return NULL;
    }

    GstFpsMonitor *monitor = GST_FPS_MONITOR(fpsmon);
    monitor->callback = &onFpsUpdateTV;
    monitor->user_data = this;
    monitor->interval = 1000; // Update every second

    GstGoenTrack *tracker = GST_GOEN_TRACK(track);
    //tracker->
    m_trackerTV = track;
    const char sTrackINIFilename[20] =  "tracking.ini";
    handle_set_cfg_filename(track, (char*)sTrackINIFilename);
    //std::strcpy(tracker->sConfigFile, "tracking.ini");



    GstCaps* caps = gst_caps_new_simple("video/x-raw",
                                        "width", G_TYPE_INT, sm->settings_store.tv_cam_width,
                                        "height", G_TYPE_INT, sm->settings_store.tv_cam_height,
                                        "format", G_TYPE_STRING, "BGR",
                                        nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr);

    GstCaps* capstrack0 = gst_caps_new_simple("video/x-raw",
                                              "width", G_TYPE_INT, sm->settings_store.tv_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.tv_cam_height,
                                              "format", G_TYPE_STRING, "RGB",
                                              nullptr);
    g_object_set(capsfiltertrack0, "caps", capstrack0, nullptr);

    GstCaps* capstrack1 = gst_caps_new_simple("video/x-raw",
                                              "width", G_TYPE_INT, sm->settings_store.tv_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.tv_cam_height,
                                              "format", G_TYPE_STRING, "RGB",
                                              nullptr);
    g_object_set(capsfiltertrack1, "caps", capstrack1, nullptr);




    gst_caps_unref(caps);
    auto videoDeviceNamePtr = QByteArray(videoDevice.toLocal8Bit());

    const gchar* video_dev = (const gchar*) (videoDeviceNamePtr).data();
    g_object_set(src, "device", video_dev,  "min-buffers", 16, nullptr);

  //  g_object_set(identity2, "drop-probability",  0.1, nullptr);
    g_object_set(queue2, "leaky", 2 ,nullptr);
    g_object_set(queue, "leaky", 2 ,nullptr);

  // g_signal_connect(identity, "fps-measurement", G_CALLBACK(fpsMeasurementCallback), this);
 //   gst_object_unref(identity);

    g_object_set(pipe, "name", "t", nullptr);

    // First caps filter after videorate
    GstCaps* caps1 = gst_caps_new_simple("video/x-raw",
                                         "framerate", GST_TYPE_FRACTION, sm->settings_store.tv_cam_target_fps, 1,
                                         nullptr);
    g_object_set(capsfilter1, "caps", caps1, nullptr);
    gst_caps_unref(caps1);



    //g_object_set(encoder, "bitrate", 8000, nullptr);

    g_object_set(encoder,
                 "bps", sm->settings_store.tv_cam_enc_target_bps,
              //   "rc-mode", sm->settings_store.tv_cam_enc_mode.toUtf8().constData(),
                 "gop", sm->settings_store.tv_cam_target_fps,
                 "bps-max", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 1.2),
                 "bps-min", (guint64)(sm->settings_store.tv_cam_enc_target_bps * 0.8),
                 nullptr);

    g_object_set(rtppay,
                 "config-interval", 1,
                 "pt", 96,
                 "aggregate-mode", 0,  // ZERO_LATENCY
                 nullptr);

    auto clientIPPtr = QByteArray(clientIP.toLocal8Bit());

    const gchar* strIP = (const gchar*) (clientIPPtr).data();

    g_object_set(sink,
                 "host", strIP,
                 "port", portUDP,
                 "sync", true,
                 "async", false,

                 nullptr);

    g_object_set(lb,
                 "device", "/dev/video60",
                 nullptr);


    // Add elements to pipeline and link
    gst_bin_add_many(GST_BIN(pipeline),src,pipe , queue, queue2, identity, identity2, capsfilter, fpsmon, track,  videorate, capsfilter1, encoder, rtppay,  sink, fakesink, nullptr);
    if (!gst_element_link_many(src, identity, capsfilter,queue,  pipe ,   videorate,  capsfilter1, encoder, rtppay, sink, nullptr) || !gst_element_link_many( pipe, queue2, identity2 ,track, fpsmon,  fakesink, nullptr)  ) { //|| !gst_element_link_many( pipe,queue2, lb, nullptr)
        g_printerr("Failed to link elements for port %d\n", portUDP);
        gst_object_unref(pipeline);
        return nullptr;
    }


    return pipeline;
}


GstElement *GST_Wrapper::run_GST_IRStreamISP(QString videoDevice, QString clientIP, qint32 portUDP)
{
    GstElement *pipeline, *src, *fpsmon,  *capsfilter, *videorate, *convert1, *capsfiltertrack0, *capsfiltertrack1, *capsfilter1, *g16, *capsfilter2, *convert2, *encoder, *rtppay, *sink;


    // // Create elements
    // GstElement* pipeline = gst_pipeline_new("video-pipeline");
    // GstElement* src = gst_element_factory_make("v4l2src", "source");
    // // GstElement* capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    // GstElement* convert = gst_element_factory_make("videoconvert", "converter");
    // GstElement* encoder = gst_element_factory_make("mpph265enc", "encoder");
    // GstElement* rtppay = gst_element_factory_make("rtph265pay", "rtppay");
    // GstElement* sink = gst_element_factory_make("udpsink", "sink");

    pipeline = gst_pipeline_new("video-pipeline");
    src = gst_element_factory_make("v4l2src", "source");
    GstElement* testsrc = gst_element_factory_make("videotestsrc", "testsource");
    GstElement* queue = gst_element_factory_make("queue", "queue");


    fpsmon = gst_element_factory_make("fpsmonitor", "fpsmon");

    capsfilter  = gst_element_factory_make("capsfilter", "caps");
    videorate = gst_element_factory_make("videorate", "rate");
    convert1 = gst_element_factory_make("videoconvert", "convert1");
    capsfilter1 = gst_element_factory_make("capsfilter", "caps1");
    g16 = gst_element_factory_make("gray16togray8", "scaler"); // Ensure you have this plugin
    capsfilter2 = gst_element_factory_make("capsfilter", "caps2");
    convert2 = gst_element_factory_make("videoconvert", "convert2");
    encoder = gst_element_factory_make("mpph265enc", "encoder");
    rtppay = gst_element_factory_make("rtph265pay", "payloader");
    sink = gst_element_factory_make("udpsink", "sink");


    // GstElement* track = gst_element_factory_make("goentrack", "track");
    capsfiltertrack0= gst_element_factory_make("capsfilter", "capstrack0");
    capsfiltertrack1= gst_element_factory_make("capsfilter", "capstrack1");
    // handle_set_cfg_filename(track, "tracking_ir.ini");


    if (!pipeline || !src  || ! fpsmon || !testsrc || !videorate || !convert1 || !capsfilter1 ||
        !g16 || !capsfilter2 || !convert2 || !encoder || !rtppay || !sink) {
        g_printerr("Failed to create elements\n");
        return nullptr;
    }

    GstFpsMonitor *monitor = GST_FPS_MONITOR(fpsmon);
    monitor->callback = &onFpsUpdateIR;
    monitor->user_data = this;
    monitor->interval = 1000; // Update every second

    g_object_set(queue, "leaky", 2 ,nullptr);


    //   GstGoenTrack *tracker = GST_GOEN_TRACK(track);
    //tracker->
    // m_trackerIR = track;

    GstCaps* caps = gst_caps_new_simple("video/x-raw",
                                     //   "format", G_TYPE_STRING, "GRAY8",
                                        "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                        "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                        //  "framerate", GST_TYPE_FRACTION, 60, 1,
                                        nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr);
    gst_caps_unref(caps);
    auto videoDeviceNamePtr = QByteArray(videoDevice.toLocal8Bit());

    const gchar* video_dev = (const gchar*) (videoDeviceNamePtr).data();
    g_object_set(src,
                 "device", video_dev,
                 "do-timestamp", TRUE,
                 "min-buffers", 7,
                 nullptr);


    // First caps filter after videorate
    GstCaps* caps1 = gst_caps_new_simple("video/x-raw",
                                         "framerate", GST_TYPE_FRACTION,  sm->settings_store.ir_cam_target_fps, 1,
                                         nullptr);
    g_object_set(capsfilter1, "caps", caps1, nullptr);
    gst_caps_unref(caps1);

    // Second caps filter after g16
    GstCaps* caps2 = gst_caps_new_simple("video/x-raw",
                                         "format", G_TYPE_STRING, "GRAY8",
                                         "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                         "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                         "framerate", GST_TYPE_FRACTION, sm->settings_store.ir_cam_target_fps, 1,
                                         nullptr);
    g_object_set(capsfilter2, "caps", caps2, nullptr);
    gst_caps_unref(caps2);

    GstCaps* capstrack0 = gst_caps_new_simple("video/x-raw",
                                              "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                              "format", G_TYPE_STRING, "RGB",
                                              nullptr);
    g_object_set(capsfiltertrack0, "caps", capstrack0, nullptr);

    GstCaps* capstrack1 = gst_caps_new_simple("video/x-raw",
                                              "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                              "format", G_TYPE_STRING, "RGB",
                                              nullptr);
    g_object_set(capsfiltertrack1, "caps", capstrack1, nullptr);



    // Configure intermediate format
    GstCaps* intermediate_caps = gst_caps_new_simple("video/x-raw",
                                                     "format", G_TYPE_STRING, "GRAY8",
                                                     "width", G_TYPE_INT, sm->settings_store.ir_cam_width * 2,
                                                     "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                                     "framerate", GST_TYPE_FRACTION, 25, 1,
                                                     nullptr);

    //g_object_set(encoder, "bitrate", 8000, nullptr);


    g_object_set(encoder,
                 "bps", sm->settings_store.ir_cam_enc_target_bps,
                // "rc-mode", sm->settings_store.ir_cam_enc_mode.toUtf8().constData(),
                 "gop", sm->settings_store.ir_cam_target_fps,
                 "bps-max", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 1.2),
                 "bps-min", (guint64)(sm->settings_store.ir_cam_enc_target_bps * 0.8),
                 nullptr);

    g_object_set(rtppay,
                 "config-interval", 1,
                 "pt", 96,
                 "aggregate-mode", 0,  // ZERO_LATENCY
                 nullptr);


    auto clientIPPtr = QByteArray(clientIP.toLocal8Bit());

    const gchar* strIP = (const gchar*) (clientIPPtr).data();

    g_object_set(sink,
                 "host", strIP,
                 "port", portUDP,
                 "sync", true,
                 "async", false,
                 nullptr);

    // Add elements to pipeline and link
    gst_bin_add_many(GST_BIN(pipeline), src, queue, fpsmon, capsfilter, videorate, convert1, capsfilter1,
                     g16,  capsfilter2, convert2, encoder, rtppay, sink, nullptr);

    if (!gst_element_link_many(src,capsfilter, queue, fpsmon,  videorate, capsfilter1, encoder, rtppay, sink, nullptr)) {
        g_printerr("Failed to link elements\n");
        gst_object_unref(pipeline);
        return nullptr;
    }

    gst_caps_unref(intermediate_caps);

    return pipeline;
}

GstElement *GST_Wrapper::run_GST_IRStream(QString videoDevice, QString clientIP, qint32 portUDP)
{
    GstElement *pipeline, *src, *fpsmon,  *capsfilter, *videorate, *convert1, *capsfiltertrack0, *capsfiltertrack1, *capsfilter1, *g16, *capsfilter2, *convert2, *encoder, *rtppay, *sink;


    // // Create elements
    // GstElement* pipeline = gst_pipeline_new("video-pipeline");
    // GstElement* src = gst_element_factory_make("v4l2src", "source");
    // // GstElement* capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    // GstElement* convert = gst_element_factory_make("videoconvert", "converter");
    // GstElement* encoder = gst_element_factory_make("mpph265enc", "encoder");
    // GstElement* rtppay = gst_element_factory_make("rtph265pay", "rtppay");
    // GstElement* sink = gst_element_factory_make("udpsink", "sink");

    pipeline = gst_pipeline_new("video-pipeline");
    src = gst_element_factory_make("v4l2src", "source");
    GstElement* testsrc = gst_element_factory_make("videotestsrc", "testsource");
    GstElement* queue = gst_element_factory_make("queue", "queue");


    fpsmon = gst_element_factory_make("fpsmonitor", "fpsmon");

    capsfilter  = gst_element_factory_make("capsfilter", "caps");
    videorate = gst_element_factory_make("videorate", "rate");
    convert1 = gst_element_factory_make("videoconvert", "convert1");
    capsfilter1 = gst_element_factory_make("capsfilter", "caps1");
    g16 = gst_element_factory_make("gray16togray8", "scaler"); // Ensure you have this plugin
    capsfilter2 = gst_element_factory_make("capsfilter", "caps2");
    convert2 = gst_element_factory_make("videoconvert", "convert2");
    encoder = gst_element_factory_make("mpph265enc", "encoder");
    rtppay = gst_element_factory_make("rtph265pay", "payloader");
    sink = gst_element_factory_make("udpsink", "sink");


   // GstElement* track = gst_element_factory_make("goentrack", "track");
    capsfiltertrack0= gst_element_factory_make("capsfilter", "capstrack0");
    capsfiltertrack1= gst_element_factory_make("capsfilter", "capstrack1");
   // handle_set_cfg_filename(track, "tracking_ir.ini");


    if (!pipeline || !src  || ! fpsmon || !testsrc || !videorate || !convert1 || !capsfilter1 ||
        !g16 || !capsfilter2 || !convert2 || !encoder || !rtppay || !sink) {
        g_printerr("Failed to create elements\n");
        return nullptr;
    }

    GstFpsMonitor *monitor = GST_FPS_MONITOR(fpsmon);
    monitor->callback = &onFpsUpdateIR;
    monitor->user_data = this;
    monitor->interval = 1000; // Update every second

    g_object_set(queue, "leaky", 2 ,nullptr);


 //   GstGoenTrack *tracker = GST_GOEN_TRACK(track);
    //tracker->
   // m_trackerIR = track;

    GstCaps* caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "GRAY8",
                                        "width", G_TYPE_INT, sm->settings_store.ir_cam_width * 2,
                                        "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                      //  "framerate", GST_TYPE_FRACTION, 60, 1,
                                        nullptr);
    g_object_set(capsfilter, "caps", caps, nullptr);
    gst_caps_unref(caps);
    auto videoDeviceNamePtr = QByteArray(videoDevice.toLocal8Bit());

    const gchar* video_dev = (const gchar*) (videoDeviceNamePtr).data();
    g_object_set(src,
                 "device", video_dev,
                 "do-timestamp", TRUE,
                 "min-buffers", 7,
                 nullptr);


    // First caps filter after videorate
    GstCaps* caps1 = gst_caps_new_simple("video/x-raw",
                                         "framerate", GST_TYPE_FRACTION, 25, 1,
                                         nullptr);
    g_object_set(capsfilter1, "caps", caps1, nullptr);
    gst_caps_unref(caps1);

    // Second caps filter after g16
    GstCaps* caps2 = gst_caps_new_simple("video/x-raw",
                                         "format", G_TYPE_STRING, "GRAY8",
                                         "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                         "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                         "framerate", GST_TYPE_FRACTION, 25, 1,
                                         nullptr);
    g_object_set(capsfilter2, "caps", caps2, nullptr);
    gst_caps_unref(caps2);

    GstCaps* capstrack0 = gst_caps_new_simple("video/x-raw",
                                              "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                              "format", G_TYPE_STRING, "RGB",
                                              nullptr);
    g_object_set(capsfiltertrack0, "caps", capstrack0, nullptr);

    GstCaps* capstrack1 = gst_caps_new_simple("video/x-raw",
                                              "width", G_TYPE_INT, sm->settings_store.ir_cam_width,
                                              "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                              "format", G_TYPE_STRING, "RGB",
                                              nullptr);
    g_object_set(capsfiltertrack1, "caps", capstrack1, nullptr);



    // Configure intermediate format
    GstCaps* intermediate_caps = gst_caps_new_simple("video/x-raw",
                                                     "format", G_TYPE_STRING, "GRAY8",
                                                     "width", G_TYPE_INT, sm->settings_store.ir_cam_width * 2,
                                                     "height", G_TYPE_INT, sm->settings_store.ir_cam_height,
                                                     "framerate", GST_TYPE_FRACTION, 25, 1,
                                                     nullptr);

    //g_object_set(encoder, "bitrate", 8000, nullptr);
    g_object_set(rtppay,
                 "config-interval", 1,
                 "pt", 96,
                 "aggregate-mode", 0,  // ZERO_LATENCY
                 nullptr);


    auto clientIPPtr = QByteArray(clientIP.toLocal8Bit());

    const gchar* strIP = (const gchar*) (clientIPPtr).data();

    g_object_set(sink,
                 "host", strIP,
                 "port", portUDP,
                 "sync", true,
                 "async", false,
                 nullptr);

    // Add elements to pipeline and link
    gst_bin_add_many(GST_BIN(pipeline), src, queue, fpsmon, capsfilter, videorate, convert1, capsfilter1,
                     g16,  capsfilter2, convert2, encoder, rtppay, sink, nullptr);

    if (!gst_element_link_many(src,capsfilter, queue, fpsmon,  videorate, convert1, capsfilter1, g16,capsfilter2, convert2, encoder, rtppay, sink, nullptr)) {
        g_printerr("Failed to link elements\n");
        gst_object_unref(pipeline);
        return nullptr;
    }

    gst_caps_unref(intermediate_caps);

    return pipeline;
}

void GST_Wrapper::proc_getOutObject()
{
    if(m_trackerTV == nullptr)
        return;

    ushort cx_out, cy_out,  w_out, h_out;

    handle_get_out_object(m_trackerTV, cx_out, cy_out,  w_out, h_out);

    emit sig_rdyOutObject(cx_out, cy_out,  w_out, h_out);
}

void GST_Wrapper::proc_getOutRoi()
{
    if(m_trackerTV == nullptr)
        return;

    ushort cx_out, cy_out,  w_out, h_out;

    handle_get_out_roi(m_trackerTV, cx_out, cy_out,  w_out, h_out);
    emit sig_rdyOutRoi(cx_out, cy_out,  w_out, h_out);
}

void GST_Wrapper::proc_getActiveLockTracking()
{
    if(m_trackerTV == nullptr)
        return;
    bool active_out, tracking_out;

    handle_get_active_lock(m_trackerTV, active_out, tracking_out);
    emit sig_rdyActiveLockTracking(active_out, tracking_out);
}


void GST_Wrapper::proc_setStopTracking()
{
    if(m_trackerTV == nullptr)
        return;

    handle_set_stop_tracking(m_trackerTV);

}

void GST_Wrapper::proc_setTargetObject(uint cx, uint cy, uint w, uint h)
{
    if(m_trackerTV == nullptr)
        return;

    handle_set_target_object(m_trackerTV, cx, cy, w, h);

}

void GST_Wrapper::proc_setRoiExt(double ext)
{
    if(m_trackerTV == nullptr)
        return;

    handle_set_roi_ext(m_trackerTV, ext);

}


