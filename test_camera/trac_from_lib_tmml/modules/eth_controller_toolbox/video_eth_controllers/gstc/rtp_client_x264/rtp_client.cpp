#include "rtp_client.hpp"

#include <gst/app/gstappsink.h>
#include "gstreamer-1.0/gst/rtp/gstrtpbuffer.h"
#include <thread>
#include <iostream>

using namespace rtp;

Fenix2Meta RtpClient::fenix2meta_data_glob = Fenix2Meta();

GstPadProbeReturn RtpClient::get_x_from_rtp_header (GstPad          *pad,
                                                GstPadProbeInfo *info,
                                                gpointer         user_data)
{
    GstBuffer *buffer;
    GstRTPBuffer rtpBuffer = GST_RTP_BUFFER_INIT;
//    guint16 ret_x;
    gpointer hdr_meta_buffer;
    GstElement* videomixer = (GstElement*)user_data;
    GstPad* videomixer_HD_sink_pad;

    // g_print("%s\n", __func__);

    buffer = GST_PAD_PROBE_INFO_BUFFER (info);
    buffer = gst_buffer_make_writable (buffer);

    /* Making a buffer writable can fail (for example if it
     * cannot be copied and is used more than once)
     */
    if (buffer == NULL)
    {
        std::cout << "WARN: metapack == null" << std::endl;
        return GST_PAD_PROBE_OK;
    }

    uint8_t * appbits = new uint8_t[1];
    uint ent_inf_sz = sizeof(fenix2meta_data_glob);

  //    uint8_t hdr_meta_buffer[ent_inf_sz];



    if (gst_rtp_buffer_map (buffer,GST_MAP_READ, &rtpBuffer))
    {
        //get x from the rtp header and into ret_x variable
        if (gst_rtp_buffer_get_extension_twobytes_header(&rtpBuffer,appbits, 1, 0, &hdr_meta_buffer, &ent_inf_sz) != TRUE)
        {
            std::cout << "WARN: failed gst_rtp_buffer_get_extension_twobytes_header" << std::endl;
            return GST_PAD_PROBE_OK;
        }
        fenix2meta_data_glob = *(reinterpret_cast<Fenix2Meta const *>(hdr_meta_buffer));
        gst_rtp_buffer_unmap (&rtpBuffer);
    }
    else
    {
        std::cout << "ERROR: gst_rtp_buffer_map failed" << std::endl;
    }

    return GST_PAD_PROBE_OK;
}


static void setUdpSrcSettings(GstElement *udpsrc,
                              const client::UdpSettings &settings)
{
    g_object_set(udpsrc, "address", settings.address.c_str(), NULL);
    g_object_set(udpsrc, "port", settings.port, NULL);
    g_object_set(udpsrc, "buffer_size", settings.bufferSize, NULL);
    g_object_set(udpsrc, "mtu", settings.mtu, NULL);
    g_object_set(G_OBJECT(udpsrc), "caps",
                 gst_caps_new_simple("application/x-rtp",
                                     "media", G_TYPE_STRING, "video",
                                     "clock-rate", G_TYPE_INT, settings.clockRate,
                                     "encoding-name", G_TYPE_STRING, settings.encodingName.c_str(),
                                     "payload", G_TYPE_INT, settings.payload, NULL),
                 NULL);
}

static void setWatchdogSettings(GstElement *watchdog, const client::WatchdogSettings &settings)
{
    g_object_set(watchdog, "timeout", settings.timeout_ms, NULL);
}

static void setAppSinkSettings(GstElement *appsink,
                               const client::VideoSettings &settings,
                               const client::AppsinkSettings &appsink_settings)
{
    g_object_set(G_OBJECT(appsink), "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "format", G_TYPE_STRING, settings.format.c_str(),
                                     "width", G_TYPE_INT, settings.width,
                                     "height", G_TYPE_INT, settings.height, NULL),
                 NULL);
    g_object_set(appsink, "max-buffers", appsink_settings.max_buffers, NULL);
    g_object_set(appsink, "emit-signals", TRUE, NULL);
}

static void setRtpDepaySettings(GstElement *rtpdepay,
                                const client::DepaySettings &settings)
{
    // g_object_set(rtpdepay, "request-keyframe", settings.requestKeyframe, NULL);
    // g_object_set(rtpdepay, "wait-for-keyframe", settings.waitForKeyframe, NULL);
}

static void setParserSettings(GstElement *parser,
                              const client::ParserSettings &settings)
{
    g_object_set(parser, "config-interval", settings.configInterval, NULL);
    // g_object_set(parser, "update-timecode", settings.updateTimecode, NULL);
}

static void setDecoderSettings(GstElement *decoder,
                               const client::DecoderSettings &settings)
{
    g_object_set(decoder, "direct-rendering", settings.directRendering, NULL);
    g_object_set(decoder, "lowres", settings.lowres, NULL);
    g_object_set(decoder, "max-threads", settings.maxThreads, NULL);
}

namespace rtp
{
    void newData(GstElement *appsink, RtpClient *client)
    {
        std::cout << "New data" << std::endl;
        std::lock_guard<std::mutex> lock(client->sync.frameMtx);
        GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
        if (sample == NULL)
        {
            fprintf(stderr, "gst_app_sink_pull_sample returned null\n");
            return;
        }
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(buffer, &client->map, GST_MAP_READ);

        gst_buffer_unmap(buffer, &client->map);
        gst_sample_unref(sample);

        client->sync.frameReady.store(true);
    }
}

RtpClient::RtpClient(client::SettingsPtr &settings) : settings(settings)
{
    frame_byte_len = settings->video.height * settings->video.width;
    if(settings->video.format == "BGR")
    {
        frame_byte_len *= 3;
    }
}

void message_ok(gboolean not_ok, std::string name)
{
    if(not_ok)
    {g_print("Can't create %s\n", name.c_str());}
    else
    {g_printerr("OK create %s\n", name.c_str());}
} // END --init_res_out

bool RtpClient::setup()
{
    gst_init(NULL, NULL);
g_printerr("START setup()\n");
    sync.frameReady.store(false);

    components.udpsrc = gst_element_factory_make("udpsrc", "udp");
    components.rtpdepay = gst_element_factory_make("rtph264depay", "depay");

//    GstElementFactory * parserfactory = gst_element_factory_find("h264parse");
//    components.parser = gst_element_factory_create(parserfactory, "parser");
    components.parser = gst_element_factory_make("h264parse", "parser");
    components.decoder = gst_element_factory_make("avdec_h264", "decoder");
    components.converter = gst_element_factory_make("videoconvert", "converter");
    components.queue = gst_element_factory_make("queue", "queue");
    components.watchdog = gst_element_factory_make("watchdog", "wdog");
    components.appsink = gst_element_factory_make("appsink", "aout");
    components.autovideosink = gst_element_factory_make("autovideosink", "avout");

    rtph264depay_sink_pad = gst_element_get_static_pad(components.rtpdepay, "sink");
    gst_pad_add_probe (rtph264depay_sink_pad, GST_PAD_PROBE_TYPE_BUFFER,
                       (GstPadProbeCallback) get_x_from_rtp_header, NULL, NULL);
    gst_object_unref (rtph264depay_sink_pad);


//    GST_ERROR()
    /// DBG::
    message_ok(!components.udpsrc, "udpsrc");
    message_ok(!components.rtpdepay, "rtpdepay");
    message_ok(!components.parser, "parser");
    message_ok(!components.decoder, "decoder");
    message_ok(!components.converter, "converter");
    message_ok(!components.queue, "queue");
    message_ok(!components.watchdog, "watchdog");
    message_ok(!components.appsink, "appsink");
    message_ok(!components.autovideosink, "autovideosink");

    pipeline = gst_pipeline_new("pipeline");

if (!pipeline) {g_printerr("!pipeline\n");}

    if (!pipeline || !components.udpsrc || !components.rtpdepay ||
        !components.parser || !components.decoder ||
        !components.converter || !components.appsink)
    {
        g_printerr("Not all elements could be created.\n");
        state.inited = false;
        g_printerr("FALSE setup()\n\n");
//        return false;
    }

    setUdpSrcSettings(components.udpsrc, settings->udp);
    setRtpDepaySettings(components.rtpdepay, settings->depay);
    setParserSettings(components.parser, settings->parser);
    setDecoderSettings(components.decoder, settings->decoder);
    setWatchdogSettings(components.watchdog, settings->wtdog);
    setAppSinkSettings(components.appsink, settings->video, settings->appsink);

    // gst_bin_add_many(GST_BIN(pipeline), components.udpsrc,
    //                  components.rtpdepay, components.parser,
    //                  components.decoder, components.converter,
    //                  components.queue, components.appsink, NULL);

        gst_bin_add_many(GST_BIN(pipeline), components.udpsrc,
                     components.rtpdepay, components.parser,
                     components.decoder, components.converter,
                     components.watchdog,
                     components.appsink, NULL);

    // gst_element_link_many(components.udpsrc,
    //                       components.rtpdepay, components.parser,
    //                       components.decoder, components.converter,
    //                       components.queue, components.appsink, NULL);

    gst_element_link_many(components.udpsrc,
                          components.rtpdepay, components.parser,
                          components.decoder, components.converter,
                          components.watchdog,
                          components.appsink, NULL);

    g_signal_connect(components.appsink, "new-sample", (GCallback)newData, this);

    state.inited = true;
g_printerr("END setup()\n");
    return true;
}

bool RtpClient::start()
{
    if (!state.inited)
    {
        if (!setup())
            return false;
    }
    sync.execute = true;
    std::thread streametTh(&RtpClient::workflow, this);
    streametTh.detach();

    return true;
}

bool RtpClient::stop()
{
    // clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    return true;
}

bool RtpClient::is_execute()
{
    return sync.execute.load();
}

bool RtpClient::frame(cv::Mat &frame, Fenix2Meta &fenix2meta_data)
{
    std::lock_guard<std::mutex> lock(sync.frameMtx);
    if (sync.frameReady.load())
    {
        memcpy(frame.data, map.data, frame_byte_len);
fenix2meta_data = fenix2meta_data_glob;
        unsigned long long int time_now = std::chrono::duration_cast<std::chrono::microseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count() % 3'600'000'000;
        fenix2meta_data.time_delay_mcs = time_now - fenix2meta_data.time_delay_mcs;
        #ifdef USE_DBG
        std::cout << "dbg:: frame[" << fenix2meta_data.num_frame << "] delay time:" << fenix2meta_data.time_delay_mcs << " [mcs]" <<  std::endl;
#endif // USE_DBG
        sync.frameReady.store(false);
        return true;
    }
    return false;
}

void RtpClient::workflow()
{
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                                 GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    // sync.frameReadyCv.notify_all();
    sync.execute = false;
    std::cout << "END workflow" << std::endl;
}
