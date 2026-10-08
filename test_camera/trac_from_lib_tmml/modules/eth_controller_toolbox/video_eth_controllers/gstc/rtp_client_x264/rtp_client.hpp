#ifndef RTP_CLIENT_HPP
#define RTP_CLIENT_HPP

#include <memory>
#include <atomic>
#include <condition_variable>

#include "../../modules/fenix2meta/fenix2meta_struct.hpp"
#include <gst/gst.h>

#include <opencv2/core.hpp>

#include "rtp_client_settings.hpp"

namespace rtp
{
    class RtpClient
    {
    public:
        RtpClient(client::SettingsPtr &settings);
        bool setup();
        bool start();
        bool stop();
        bool is_execute();
        bool frame(cv::Mat &frame, Fenix2Meta &fenix2meta_data);
//time_data meta_cli;
//    Fenix2Meta fenix2meta_data;
    private:
        friend void newData(GstElement *appsrc, RtpClient *client);
        void workflow();

        GstElement *pipeline = nullptr;
        struct PipelineComponents
        {
            GstElement *udpsrc = nullptr;
            GstElement *rtpdepay = nullptr;
            GstElement *parser = nullptr;
            GstElement *decoder = nullptr;
            GstElement *converter = nullptr;
            GstElement *queue = nullptr;
            GstElement *watchdog = nullptr;
            GstElement *appsink = nullptr;
            GstElement *autovideosink = nullptr;
        } components;
        client::SettingsPtr settings;
        GstBuffer *framebuffer = nullptr;
        GstMapInfo map;
        int frame_byte_len = 0;
        struct Flags
        {
            bool inited = false;

        } state;
        struct Synchronisation
        {
            std::condition_variable frameReadyCv;
            std::mutex frameMtx;
            std::atomic<bool> frameReady = {false};
            std::atomic<bool> execute = {false};
        } sync;

    ///meta data in header
    GstPad *rtph264depay_sink_pad = nullptr;

    static GstPadProbeReturn get_x_from_rtp_header (GstPad          *pad,
                                                    GstPadProbeInfo *info,
                                                    gpointer         user_data);
    static std::atomic<bool> new_frame;
    static Fenix2Meta fenix2meta_data_glob;

    };
}

#endif // RTP_CLIENT_HPP
