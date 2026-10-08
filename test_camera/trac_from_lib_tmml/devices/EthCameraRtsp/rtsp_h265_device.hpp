#ifndef RTSP_H265_DEVICE_HPP
#define RTSP_H265_DEVICE_HPP


#include "devices/device.hpp"
#include "rtsp_h265_device_settings.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>
#include <string>

#include <gst/gst.h>
#include <gst/app/app.h>
#include <opencv2/opencv.hpp>



#include <opencv2/core.hpp>


namespace devices
{
class RTSP_H265_device : public Device
{
public:
    explicit RTSP_H265_device(rtsp_h265_device::SettingsPtr &settings);
    ~RTSP_H265_device();
    void setup() override;
    void start() override;
    void quit() override;

    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
    int getColorChannels() override;
    bool isBayerColorChannel() override;
    void keyHandler(unsigned char &key) override;
    void workflow() override;
    bool getState() override {return true;}
private:
    friend void newData(GstElement *appsrc,
                        RTSP_H265_device *client);
    void exec();
    void runHandleFrame();

    rtsp_h265_device::SettingsPtr settings;
//    GstElement *pipeline = nullptr;
//    struct PipelineComponents
//    {
//        GstElement *nvarguscamerasrc = nullptr;
//        GstElement *nvvidconv = nullptr;
//        GstElement *inputSettingsSetter = nullptr;
//        GstElement *appsink = nullptr;
//    } components;
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
    bool first_frame = 1;
    cv::Mat frame;
    std::string output_format_gst;
    int frame_byte_length = 0;
    std::string pipeline_str = "";
    GError *error = nullptr;
    GstElement *pipeline = nullptr;
    GstElement *sink = nullptr;
};
}; // namespace devices

#endif // RTSP_H265_device_HPP
