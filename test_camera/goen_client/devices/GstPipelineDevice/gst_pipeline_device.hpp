#ifndef GST_PIPELINE_DEVICE_HPP
#define GST_PIPELINE_DEVICE_HPP


#include "devices/device.hpp"
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
#include <thread>

#include <gst/app/gstappsink.h>




namespace devices
{
static     std::atomic<bool> f_state = {false};


class GST_PIPELINE_device : public Device
{
public:
    explicit GST_PIPELINE_device(std::string & pipeline);
    ~GST_PIPELINE_device();
    void setup() override;
    void start() override;
    void quit() override;

    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
    int getColorChannels() override;
    bool isBayerColorChannel() override;
    void keyHandler(unsigned char &key) override;
    void workflow() override;
    bool getState() override {
//        GstState * state;
//        GstClockTime timeout = 1000;
//        GstStateChangeReturn state_change = gst_element_get_state(GST_ELEMENT(pipeline), state, state, 1000);
//        return state_change;
        return f_state.load();
    }
private:
    friend void newData(GstElement *appsrc,
                        GST_PIPELINE_device *client);
    void exec();
    void runHandleFrame();

    int fps = 0;
    int width = 0;
    int height = 0;

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

#endif // GST_PIPELINE_device_HPP
