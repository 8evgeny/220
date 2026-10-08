#include "RtpGstServer.hpp"
#include "gst/app/gstappsrc.h"

#include <gst/gst.h>

#include <gstreamer-1.0/gst/gst.h>
#include <gstreamer-1.0/gst/gstelement.h>
#include <iostream>
#include <ostream>
#include <sstream>

#include <thread>

static bool ifGstInited = false;

using namespace rtp;

static void freeGstElement(GstElement *&el)
{
    if (el){
        gst_object_unref(el);
        el = nullptr;
    }
}

static void printPiplineSettings(std::shared_ptr<rtp::server::Settings> settings)
{
//    std::clog << "################################################################\n"
//                 "Input formast:\n"
//                 "media type         = " << settings->video.media_type << '\n' <<
//                 "input video format = " << settings->video.format     << '\n' <<
//                 "input video color = "  << settings->video.color      << '\n' <<
//                 "width              = " << settings->video.width      << '\n' <<
//                 "height             = " << settings->video.height     << '\n' <<
//                 "Encoder settings:"     << '\n' <<
//                 "bitrate      = "       << settings->encoder.bitrate / 1024      << " [kbit/s]\n" <<
//                 "peak-bitrate = "       << settings->encoder.peek_bitrate / 1024 << " [kbit/s]\n" <<
//                 "preset-level = "       << settings->encoder.preset              << '\n' <<
//                 "insert-sps-pps = "     << settings->encoder.insert_sps_pps      << '\n' <<
//                 "insert-vui = "         << settings->encoder.insert_vui          << '\n' <<
//                 "UDP settings:"         << '\n' <<
//                 "bitrate = "            << settings->udp.host << "\n" <<
//                 "port    = "            << settings->udp.port << "\n" <<
//                 "################################################################"
//              << std::endl;
}

RtpServer::RtpServer()
    : _settings(new rtp::server::Settings)
    , _pipeline{nullptr}
    , _appsrc{nullptr}
    , _running{false}

{
    if(!ifGstInited) {
        // инициализируем gstreamer
        gst_init(NULL, NULL);
        ifGstInited = true;
    }
}

RtpServer::~RtpServer()
{
    if (_pipeline){
        gst_element_set_state(_pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(_pipeline));
    }
    _pipeline = nullptr;
    freeGstElement(_appsrc);
    /**
     * Clean up any resources created by GStreamer in gst_init.
     * It is normally not needed to call this function in a normal application
     * as the resources will automatically be freed when the program terminates.
     * This function is therefore mostly used by testsuites and other memory profiling tools.
     * After this call GStreamer (including this method) should not be used anymore.
     **/
//    gst_deinit();
}

bool RtpServer::setup()
{
    std::lock_guard<std::mutex> lock(_mutex);
    bool result = false;
    if (_pipeline){
        std::cerr << "Видеоконвеер был создан ранее" << std::endl;
    } else {
        printPiplineSettings(_settings);

        /* SOURCE */
        constexpr char srcStr[]  = " appsrc name=mysrc format=GST_FORMAT_BYTES is-live=true caps=";
        std::stringstream  srcCapsStr("");
        srcCapsStr  <<_settings->video.media_type
                    << ",format="    << _settings->video.format.c_str()
                    << ",width="     << _settings->video.width
                    << ",height="    << _settings->video.height
                    << ",framerate=" << _settings->video.fps << "/1";
        /* DEBAYERING */
        std::string debayeringStr("");
        if(_settings->video.color) {
            debayeringStr = "bayer2rgb ! ";
        } else {
            debayeringStr = "";
        }
        std::stringstream encoderSettingsStr(" ");
#ifdef NVJETSON
        /* CONVERTER */
        constexpr char videoconvertStr[] = "nvvideoconvert  flip-method=2 ! video/x-raw(memory:NVMM), format=I420, colorimetry=bt601";
        /* ENCODER */
        constexpr char encoderStr[] = "nvv4l2h265enc";
        encoderSettingsStr << " control-rate="   << _settings->encoder.control_rate
                           << " bitrate="        << _settings->encoder.bitrate
                           << " peak-bitrate="   << _settings->encoder.peek_bitrate
                           << " preset-level="   << _settings->encoder.preset
                           << " insert-sps-pps=" << (_settings->encoder.insert_sps_pps ? "true" : "false")
                           << " insert-vui="     << (_settings->encoder.insert_vui ? "true" : "false");
        constexpr char parseStr[] = "h265parse config_interval=-1";
        /* RTP */
        constexpr char rtphStr[] = "rtph265pay";
#else
        /* CONVERTER */
        constexpr char videoconvertStr[] = "videoconvert";
        /* ENCODER */
        constexpr char encoderStr[] = "x264enc";
        encoderSettingsStr << " bitrate="         << _settings->encoder.bitrate / 1024
                           << " b-adapt="        << "true"
                           << " tune="           << "zerolatency"
                           << " threads="        << "1"
                           << " key-int-max="    << "15"
                           << " speed-preset="   << "ultrafast"
                           << " intra-refresh="  << "true"
                           << " insert-vui="     << (_settings->encoder.insert_vui ? "true" : "false");
        constexpr char parseStr[] = "h264parse config_interval=-1";
        /* RTP */
        constexpr char rtphStr[] = "rtph264pay";
#endif
        std::stringstream rtphSettingsStr(" ");
        rtphSettingsStr << " mtu=" << _settings->pay.mtu
                        << " pt=" << "96"
                        << " config-interval=" << "1";
        /* UDP SEND */
        constexpr char udpsinkStr[] = "udpsink";
        std::stringstream udpsinkSettingsStr(" ");
        udpsinkSettingsStr << "host="   << _settings->udp.host
                           << " port="  << _settings->udp.port
                           << " async=" << "false"
                           << " sync="  << "false";
        /* PIPLINE */
        std::stringstream pipelineStr("");
        pipelineStr << srcStr << srcCapsStr.str() << " ! "
                    << debayeringStr
                    << videoconvertStr << " ! "
                    << encoderStr      << " " << encoderSettingsStr.str() << " ! "
                    << parseStr        << " ! "
                    << rtphStr         << " " << rtphSettingsStr.str()    << " ! "
                    << udpsinkStr      << " " << udpsinkSettingsStr.str();
//        std::clog << pipelineStr.str() << std::endl;

        GError *error = nullptr;
        //создадим конвеер
        _pipeline = gst_parse_launch((gchar*)pipelineStr.str().c_str(), &error);
        if (error){
            std::cerr << "Ошибка конвеера gstreamer: " << error->message << std::endl;
        } else if (!_pipeline){
            std::cerr << "Неудалось создать видеоконвеер" << std::endl;
        } else {
            freeGstElement(_appsrc);
            //попробуем получить наш источник, через который будем пихать кадры
            _appsrc = gst_bin_get_by_name(GST_BIN(_pipeline),"mysrc");
            if (!_appsrc){
                std::cerr << "Ошибка получения элемента [appsrc]" << std::endl;
            } else {
//                gst_app_src_set_stream_type(GST_APP_SRC(_appsrc), GST_APP_STREAM_TYPE_STREAM);
                gst_app_src_set_size(GST_APP_SRC(_appsrc), -1);
//                g_object_set(G_OBJECT(_appsrc), "format" , GST_FORMAT_BUFFERS, NULL);
//                g_object_set(G_OBJECT(_appsrc), "block"  , true, NULL);
//                g_object_set(G_OBJECT(_appsrc), "is-live", false, NULL);
                //попробуем запустить конвеер
                GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);
                switch (ret) {
                    case GST_STATE_CHANGE_FAILURE:{
                        std::cerr << "Ошибка запуска конвеера [GST_STATE_CHANGE_FAILURE]\n" << std::endl;
                    } break;
                    case GST_STATE_CHANGE_SUCCESS:{
                        std::clog << "Запуск конвеера [GST_STATE_CHANGE_SUCCESS]\n" << std::endl;
                        result = true;
                    } break;
                    case GST_STATE_CHANGE_ASYNC:{
                        std::cerr << "Запуск конвеера [GST_STATE_CHANGE_ASYNC]\n" << std::endl;
                    } break;
                    case GST_STATE_CHANGE_NO_PREROLL:{
                        std::cerr << "Запуск конвеера [GST_STATE_CHANGE_NO_PREROLL]\n" << std::endl;
                    } break;
                    default: std::cerr << "Неизвестное состояние конвеера\n" << std::endl;
                    break;
                }
            }
        }
    }
    return result;
}

bool RtpServer::start()
{
    _running = false;
    if (setup()) {
        _running = true;
    }
    return _running;
}

bool RtpServer::stop()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_pipeline){
            GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_NULL);
            switch (ret) {
                case GST_STATE_CHANGE_FAILURE:{
                    std::cerr << "Ошибка остановки конвеера [GST_STATE_CHANGE_FAILURE]" << std::endl;
                } break;
                case GST_STATE_CHANGE_SUCCESS:{
                     std::clog << "Остановка конвеера [GST_STATE_CHANGE_SUCCESS]" << std::endl;
                     _running = false;
                } break;
                default: {
                    std::cerr << "Неизвестное состояние конвеера [GST_?]" << std::endl;
                } break;
            }
            gst_object_unref(GST_OBJECT(_pipeline));
            _pipeline = nullptr;
    }
    freeGstElement(_appsrc);
    return true;
}

void RtpServer::newFrame(uint8_t *buffer, size_t size)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(!buffer) {
        std::cerr << "Буффер кадра == nullptr" << std::endl;
    } else if(!_pipeline) {
        std::cerr << "_pipeline == nullptr" << std::endl;
    } else if(!_appsrc) {
        std::cerr << "_appsrc == nullptr" << std::endl;
    } else {
        GstBuffer *gstBuffer = gst_buffer_new_allocate(0, size, 0);
        if (!gstBuffer){
            std::cerr << "gst_buffer_new_allocate неудачно" << std::endl;
        } else {
            GstMapInfo info;
            if(!gst_buffer_map(gstBuffer, &info, GstMapFlags(GST_MAP_WRITE))) {
                gst_buffer_unref(gstBuffer);
            } else {
                memcpy(info.data, (guint8*)buffer, size);
                gst_buffer_unmap(gstBuffer, &info);
                GstCaps* caps = gst_caps_new_simple (_settings->video.media_type.c_str(),
                                                     "format",      G_TYPE_STRING,      _settings->video.format.c_str(),
                                                     "width",       G_TYPE_INT,         _settings->video.width,
                                                     "height",      G_TYPE_INT,         _settings->video.height,
                                                     "framerate",   GST_TYPE_FRACTION,  _settings->video.fps, 1,
                                                     NULL);
                if (!caps){
                    gst_buffer_unref(gstBuffer);
                    std::cerr << "gst_caps_new_simple неудачно" << std::endl;
                } else {
                    GstSample *sample = gst_sample_new(gstBuffer, caps, NULL, NULL);
//                    GstSample *sample = gst_sample_new(gstBuffer, NULL, NULL, NULL);
                    if (!sample){
                        gst_buffer_unref(gstBuffer);
                        gst_caps_unref(caps);
                        std::cerr << "gst_sample_new неудачно" << std::endl;
                    } else {
                        gst_app_src_push_sample(GST_APP_SRC(_appsrc),sample);
                        gst_sample_unref(sample);
                        gst_buffer_unref(gstBuffer);
                        gst_caps_unref(caps);
                    }
                }

            }

        }

    }
}
