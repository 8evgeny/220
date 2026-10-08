#ifndef RaspberryHQCamera_HPP
#define RaspberryHQCamera_HPP

#include "devices/device.hpp"
#include "settings.hpp"

#include <gst/gst.h>
#include <opencv2/core.hpp>

#include <atomic>
#include <condition_variable>

namespace devices
{
class RaspberryHQCamera : public Device
{
public:
    explicit RaspberryHQCamera(raspberry_hq_camera::SettingsPtr &settings);
    ~RaspberryHQCamera();
    void setup() override;
    void start() override;
    void quit() override;

    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
    int getColorChannels() override;
    bool isBayerColorChannel() override;
    void keyHandler(char &key) override;
    void workflow() override;
private:
    friend void newData(GstElement *appsrc,
                        RaspberryHQCamera *client);
    void exec();
    void runHandleFrame();

    raspberry_hq_camera::SettingsPtr settings;
    GstElement *pipeline = nullptr;
    struct PipelineComponents
    {
        GstElement *nvarguscamerasrc = nullptr;
        GstElement *nvvidconv = nullptr;
        GstElement *inputSettingsSetter = nullptr;
        GstElement *appsink = nullptr;
    } components;
    GstBuffer *framebuffer = nullptr;
    GstMapInfo map;
    int frameCnt = 0;
    struct Flags
    {
        bool inited = false;

    } state;
    struct Synchronisation
    {
        std::condition_variable frameReadyCv;
        std::mutex frameMtx;
        std::atomic<bool> frameReady;
        std::atomic<bool> handleFrameExecute = false;
    } sync;
    cv::Mat frame;
    std::string output_format_gst;
};
};

#endif // RaspberryHQCamera_HPP
