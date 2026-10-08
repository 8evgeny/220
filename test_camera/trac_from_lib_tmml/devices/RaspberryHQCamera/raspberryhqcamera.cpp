#include "raspberryhqcamera.hpp"

#include <thread>

#include <gst/app/gstappsink.h>

#ifdef GUI_OK
#include <opencv2/highgui.hpp>
#endif //GUI_OK
#include <opencv2/imgproc.hpp>

using namespace devices;
using namespace devices::raspberry_hq_camera;

static void setNvArgusCameraSrcSettings(GstElement *nvarguscamerasrc,
                                        GstElement *capsfilter,
                                        Settings &settings)
{
    g_object_set(nvarguscamerasrc, "sensor-id", settings.camera.sensor_id, NULL);
    //    g_object_set(nvarguscamerasrc, "bufapi-version", settings.camera.bufapi_version, NULL);
    g_object_set(nvarguscamerasrc, "wbmode", settings.camera.wbmode, NULL);
    g_object_set(nvarguscamerasrc, "sensor-mode", settings.camera.sensor_mode, NULL);
    g_object_set(G_OBJECT(capsfilter), "caps",
                 gst_caps_new_simple("video/x-raw(memory:NVMM)",
                                     "format", G_TYPE_STRING, settings.video.format.c_str(),
                                     "width", G_TYPE_INT, settings.video.width,
                                     "height", G_TYPE_INT, settings.video.height,
                                     "framerate", G_TYPE_STRING, std::string(std::to_string(settings.video.fps) + "/1").c_str(),
                                     NULL),
                 NULL);
}

static void setNvVidConvSrcSettings(GstElement *nvvidconv)
{
    g_object_set(nvvidconv, "flip-method", 2, NULL);
}

static void setAppSinkSettings(GstElement *appsink, OutputSettings &settings)
{
    g_object_set(G_OBJECT(appsink), "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "format", G_TYPE_STRING, settings.format.c_str(),
                                     "width", G_TYPE_INT, settings.width,
                                     "height", G_TYPE_INT, settings.height,
                                     NULL),
                 NULL);
    g_object_set(appsink, "emit-signals", TRUE, NULL);
}

namespace devices
{
void newData(GstElement *appsink, RaspberryHQCamera *client)
{
    std::lock_guard lock(client->sync.frameMtx);
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

    client->frameCnt++;
    client->sync.frameReady.store(true);
    client->sync.frameReadyCv.notify_one();
}
}

RaspberryHQCamera::RaspberryHQCamera(SettingsPtr &settings) : settings(settings)
{
#if defined(CCM_8UC3)
    settings->output.format = "RGBA";
#elif defined(CCM_8UC1)
    settings->output.format = "GRAY8";
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
}

RaspberryHQCamera::~RaspberryHQCamera()
{
    quit();
}

void RaspberryHQCamera::setup()
{
    gst_init(NULL, NULL);

    sync.frameReady.store(false);

    components.nvarguscamerasrc = gst_element_factory_make("nvarguscamerasrc", "camerasrc");
    components.inputSettingsSetter = gst_element_factory_make("capsfilter", "inSettingsSetter");
    components.nvvidconv = gst_element_factory_make("nvvidconv", "nvvidconv");
    components.appsink = gst_element_factory_make("appsink", "aout");

    pipeline = gst_pipeline_new("pipeline");

    if (!pipeline ||
            !components.nvarguscamerasrc ||
            !components.inputSettingsSetter ||
            !components.nvvidconv ||
            !components.appsink)
    {
        g_printerr("Not all elements could be created.\n");
        state.inited = false;
        return;
    }

    setNvArgusCameraSrcSettings(components.nvarguscamerasrc,
                                components.inputSettingsSetter,
                                *settings);
    setNvVidConvSrcSettings(components.nvvidconv);
    setAppSinkSettings(components.appsink, settings->output);

    gst_bin_add_many(GST_BIN(pipeline),
                     components.nvarguscamerasrc,
                     components.inputSettingsSetter,
                     components.nvvidconv,
                     components.appsink, NULL);
    gst_element_link_many(components.nvarguscamerasrc,
                          components.inputSettingsSetter,
                          components.nvvidconv,
                          components.appsink, NULL);

    g_signal_connect(components.appsink, "new-sample", (GCallback)newData, this);

    state.inited = true;
}

void RaspberryHQCamera::start()
{
    if (!state.inited)
    {
        setup();
        if (!state.inited)
            return;
    }
    sync.handleFrameExecute.store(true, std::memory_order_release);
    std::thread(&RaspberryHQCamera::exec, this).detach();
    std::thread(&RaspberryHQCamera::runHandleFrame, this).detach();
}

void RaspberryHQCamera::quit()
{
    sync.handleFrameExecute.store(false, std::memory_order_acquire);
    sync.frameReadyCv.notify_one();

    if(pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
        pipeline = nullptr;
    }
}

uint8_t *RaspberryHQCamera::receiveFrame(int &w, int &h, int &id, int &num)
{
    if (sync.frameReady.load())
    {
        w = settings->output.width;
        h = settings->output.height;
        id = 0;
        num = frameCnt;
        sync.frameReady.store(false);
        return map.data;
    };
    return nullptr;
}

void RaspberryHQCamera::getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
{
#if defined(CCM_8UC3)
    static cv::Mat tempImg(settings->output.height, settings->output.width, CV_8UC4);
    memcpy(tempImg.data, map.data, map.size);
    cv::cvtColor(tempImg, image, cv::COLOR_RGBA2BGR);
#elif defined(CCM_8UC1)
    memcpy(image.data, map.data, map.size);
#else 
    throw std::runtime_error("Not supported color space for output format");
#endif
    sync.frameReady.store(false);
}

int RaspberryHQCamera::getColorChannels()
{
#if defined(CCM_8UC3)
    return 3;
#elif defined(CCM_8UC1)
    return 1;
#else
    return 0;
#endif
}

bool RaspberryHQCamera::isBayerColorChannel()
{
    return false;
}

void RaspberryHQCamera::keyHandler(uchar &key)
{

}

void RaspberryHQCamera::workflow()
{

}

void RaspberryHQCamera::exec()
{
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    //    GstBus *bus = gst_element_get_bus(pipeline);
    //    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
    //                                                 GstMessageType(GST_MESSAGE_ERROR));
}

void RaspberryHQCamera::runHandleFrame()
{
    while(sync.handleFrameExecute.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lk(sync.frameMtx);
        sync.frameReadyCv.wait(lk);
        if(!sync.handleFrameExecute.load(std::memory_order_acquire))
        {
            break;
        }
        for(auto handler : frame_handlers)
        {
            handler->handle(map.data, settings->output.width,
                            settings->output.height, 0, frameCnt);
        } // -- END for(auto handler : frame_handlers)
        lk.unlock();
    } // -- END while
}

