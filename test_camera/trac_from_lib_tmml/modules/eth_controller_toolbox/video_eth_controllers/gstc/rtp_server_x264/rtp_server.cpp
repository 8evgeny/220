#include "rtp_server.hpp"

#include <thread>

#include <iostream>

#include "gstreamer-1.0/gst/rtp/gstrtpbuffer.h"

#include <functional>
#include <memory>

using namespace rtp;

std::atomic<bool> RtpServer::new_frame = {false};
Fenix2Meta RtpServer::meta_data_srv_glob = Fenix2Meta();

static void setAppSrcSettings(GstElement *appsrc,
                              const server::VideoSettings &settings)
{
    g_object_set(G_OBJECT(appsrc), "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "format", G_TYPE_STRING, settings.format.c_str(),
                                     "width", G_TYPE_INT, settings.width,
                                     "height", G_TYPE_INT, settings.height, NULL),
                                    //  "framerate", GST_TYPE_FRACTION, 0, settings.framerate, NULL),
                 NULL);

    g_object_set(appsrc, "emit-signals", TRUE, NULL);

    /* this instructs appsrc that we will be dealing with timed buffer */
    gst_util_set_object_arg(G_OBJECT(appsrc), "format", "time");
}

static void setRtpPaySettings(GstElement *rtppay,
                              const server::PaySettings &settings)
{
    g_object_set(rtppay, "config-interval", settings.configInterval, NULL);
    g_object_set(rtppay, "pt", settings.pt, NULL);
    g_object_set(rtppay, "mtu", settings.mtu, NULL);
}

static void setUdpSettings(GstElement *udpsink,
                           const server::UdpSettings &settings)
{
    g_object_set(udpsink, "host", settings.host.c_str(), NULL);
    g_object_set(udpsink, "port", settings.port, NULL);
}

static void setEncoderSettings(GstElement *encoder,
                                const server::EncoderSettings &settings)
{
    g_object_set(encoder, "bitrate", settings.bitrate, NULL);
    
    g_object_set(encoder, "tune", 4, NULL); // zerolatency=4, fastdecode=2
    g_object_set(encoder, "speed-preset", 3, NULL); // 1 - ulatrafast; 9 - veryslow
}

GstPadProbeReturn rtp::RtpServer::set_x_to_rtp_header1(GstPad *pad,
                                                       GstPadProbeInfo * info,
                                                       gpointer     user_data)
{
    if(!new_frame.load())
    {
        #ifdef USE_DBG
        g_print("skip...\n");
#endif // USE_DBG
        return GST_PAD_PROBE_OK;
    }
    new_frame.store(false);

    GstBuffer *buffer;
    GstRTPBuffer rtp_buffer = GST_RTP_BUFFER_INIT;
    //    g_print("%s\n", __func__);

    buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    buffer = gst_buffer_make_writable(buffer);

    if(buffer == NULL)
    {
        return GST_PAD_PROBE_OK;
    }

    ///serialize time data

    uint8_t hdr_meta_buffer[sizeof(meta_data_srv_glob)];
    memcpy(hdr_meta_buffer, reinterpret_cast<uint8_t const *>(&meta_data_srv_glob), sizeof(meta_data_srv_glob));
    if (gst_rtp_buffer_map(buffer, GST_MAP_WRITE, &rtp_buffer))
    {
        gst_rtp_buffer_add_extension_twobytes_header(&rtp_buffer, 1, 1, hdr_meta_buffer, sizeof(meta_data_srv_glob));
        gst_rtp_buffer_unmap(&rtp_buffer);
    }
    else
    {
        g_print("ERROR: attach metadata to rtp pack failed!");
    }


    return GST_PAD_PROBE_OK;
} // -- END set_x_to_rtp_header1

namespace rtp
{
    void waitData(GstElement *appsrc, guint unused, rtp::RtpServer *server)
    {
        static GstClockTime timestamp = 0;
        static int cnt = 0;
        std::unique_lock<std::mutex> lock(server->sync.frameMtx);
        server->sync.frameReadyCv.wait(lock);
        if(!server->sync.execute.load())
        {
            return;
        }
        GstFlowReturn ret;
        GST_BUFFER_PTS(server->framebuffer) = timestamp;
        GST_BUFFER_DURATION(server->framebuffer) =
            gst_util_uint64_scale_int(1, GST_SECOND,
                                      server->settings->video.framerate - 1);
        timestamp += GST_BUFFER_DURATION(server->framebuffer);
        ;

        g_signal_emit_by_name(appsrc, "push-buffer", server->framebuffer, &ret);
    }
}

RtpServer::RtpServer(server::SettingsPtr &settings_ext) //: settings(settings)
{
settings = settings_ext;
}

bool RtpServer::setup()
{
    gst_init(NULL, NULL);

    framebuffer = gst_buffer_new_allocate(NULL,
                                          settings->video.width * settings->video.height * 3,
                                          NULL);

    components.appsrc = gst_element_factory_make("appsrc", "as");
    components.converter = gst_element_factory_make("videoconvert", "converter");
    components.encoder = gst_element_factory_make("x264enc", "enc");
    components.rtppay = gst_element_factory_make("rtph264pay", "rtphpay");

    rtph264pay_pad = gst_element_get_static_pad(components.rtppay, "src");
    gst_pad_add_probe (rtph264pay_pad, GST_PAD_PROBE_TYPE_BUFFER,
                       (GstPadProbeCallback) set_x_to_rtp_header1, NULL, NULL);
    gst_object_unref (rtph264pay_pad);

    components.udpsink = gst_element_factory_make("udpsink", "udp");

    pipeline = gst_pipeline_new("pipeline");

    if (!pipeline || !components.appsrc || !components.converter || !components.encoder ||
        !components.rtppay || !components.udpsink)
    {
        g_printerr("Not all elements could be created.\n");
        state.inited = false;
        return false;
    }

    setAppSrcSettings(components.appsrc, settings->video);
    setRtpPaySettings(components.rtppay, settings->pay);
    setUdpSettings(components.udpsink, settings->udp);
    setEncoderSettings(components.encoder, settings->encoder);

    g_signal_connect(components.appsrc, "need-data", GCallback(waitData), this);

    gst_bin_add_many(GST_BIN(pipeline), components.appsrc, components.converter,
                     components.encoder, components.rtppay, components.udpsink, NULL);
    gst_element_link_many(components.appsrc, components.converter, components.encoder,
                          components.rtppay, components.udpsink, NULL);

    state.inited = true;
    return true;
}

bool RtpServer::start()
{
    if (!state.inited)
    {
        if (!setup())
            return false;
    }
    
    sync.execute.store(true);
    std::thread streametTh(&RtpServer::workflow, this);
    streametTh.detach();
    
    return true;
}

std::function<void(GstElement* element, void* userData)> std_pipeline_set_null;
void pipeline_set_null(GstElement* element, void* userData)
{
    std_pipeline_set_null(element, userData);
}

bool RtpServer::stop()
{
    sync.execute.store(false);
    sync.frameReadyCv.notify_all();
    // clean up
    // gst_element_send_event ((GstElement*)pipeline, gst_event_new_eos ());
    // bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));

    // gst_message_unref (msg);
    // GstEvent*  event = gst_event_new_eos();
    // gst_element_send_event(pipeline, event);
    // gst_bus_post (bus, gst_message_new_eos(NULL));
    // gst_object_unref (bus);

    if(!gst_bus_post (bus, gst_message_new_eos(NULL)))
    {
        std::cout << "ERROR bus_post EOS" << std::endl;
    }
    else
    {
        std::cout << "SUCCESS bus_post EOS" << std::endl;
    }

    // gst_element_set_state(pipeline, GST_STATE_NULL); // прямой вызов может спровоцировать dead-lock, т.к. потоконебезопасно
    // Нужно действовать через gst_element_call_async

    
    std_pipeline_set_null = 
        [](GstElement* element, void* userData)
        {
            GstElement* ppln = reinterpret_cast<GstElement*>(userData);
            if(ppln != nullptr)
            {
                std::cout << "LAUNCH gst_element_set_state(ppln, GST_STATE_NULL);" << std::endl;
                // gst_element_set_state(ppln, GST_STATE_NULL);
                gst_object_unref(ppln);
                std::cout << "Success GST_STATE_NULL pipeline" << std::endl;
            }
            else
            {
                std::cout << "Error cast pointer" << std::endl;
            }
        };

    // void* userData = reinterpret_cast<void*>(pipeline);
    // GDestroyNotify destrNoty;
    // std::cout << "BEFORE call async" << std::endl;
    // gst_element_call_async(pipeline, pipeline_set_null, userData, destrNoty);
    // std::cout << "AFTER call async" << std::endl;

    std::this_thread::sleep_for(std::chrono::microseconds(100));
    // gst_object_unref(pipeline);
    
    return true;
}

void RtpServer::newFrame(uint8_t *buffer, size_t size)
{
    static unsigned int cnt = 0;
//    uint frame_num = cnt;

    std::lock_guard<std::mutex> lock(sync.frameMtx);
    gsize gst_size_buffer = gst_buffer_fill(framebuffer, 0, buffer, size);
//   g_print("dbg:: buff size = %lu\n", gst_size_buffer);
        new_frame.store(true);
    cnt++;
    sync.frameReadyCv.notify_all();
} // END newFrame

void RtpServer::setMeta(Fenix2Meta & meta_ext)
{
    meta_data_srv_glob = meta_ext;
    return;
} // -- END setMeta

void RtpServer::workflow()
{
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                    GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    std::cout << "END workflow" << std::endl;
}
