#ifndef WEBCAMERA_HPP
#define WEBCAMERA_HPP
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <atomic>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include "tools/watchdog.h"
#include "tools/INIReader.h"
#include "devices/device.hpp"
#ifdef GUI_OK
   #include <opencv2/highgui.hpp>
#endif // END #ifdef GUI_OK

enum PixelFormat
{
    BACKEND_GSTREAMER   = 0,
    BACKEND_V4L2        = 1,
    BACKEND_LATEST      = 2
}; // END enum PixelFormat

class WebCamera : public Device
{
 public:
    WebCamera(){}
    ~WebCamera();
    WebCamera(const std::string &path_to_ini, bool &ok, int & dev_fps);
    // ===================================
    void setup() override {return;};
    bool getState() override {return true;}    
    int getFrameCounter() override;
    void keyHandler(uchar &key) override;
    void workflow() override;
    int getColorChannels() override;
    bool isBayerColorChannel() override;
    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    void quit() override;    
    int getId() override;

 private:
    int crop_l = 960;
    int crop_t = 540;
    int crop_r = 2880; // crop_l + 1920
    int crop_b = 1620; // crop_t + 1080
    int crop_w = 1920;
    int crop_h = 1080;

    cv::Rect crop_center = cv::Rect(crop_l, crop_t, crop_w, crop_h);

    std::unique_ptr<cv::VideoCapture> cap_ptr = nullptr;
    std::atomic<bool> _execute = false; // флаг нахождения программы в процессе выполнения
    std::atomic<bool> handle_frame_execute = false;
    std::atomic<bool> frame_ready = false;
    std::chrono::system_clock::time_point tp_get_fr, tp_get_fr_cur;
    std::mutex frame_mutex;
    std::condition_variable frameCV;    
    int sourceFrameDelay_ms = 0; // временной интервал между кадрами, мс (обратно FPS )
    cv::Mat frame;
    int frame_w = 1080 ;
    int frame_h = 1080 ;
    int fps = 30;
    int backend_id = 0;
    int frameCnt = 0;    
    int change_color = 0;
    int channels_in = 0;
    int rotate180 = 0;

#if defined(CCM_8UC1)
    cv::Mat img_UHD = cv::Mat(cv::Size(3840, 2160), CV_8UC1);
#else
    cv::Mat img_UHD = cv::Mat(cv::Size(3840, 2160), CV_8UC3);    
#endif // END #if not defined(CCM_8UC1)

#if defined USE_TV
    int webcamera_id_TV = -1;
#endif // END #if defined USE_TV
#if defined USE_TPV_cam
    int webcamera_id_TPV = -1;
#endif // #if defined USE_TPV_cam
    unsigned char key_quit_handler = '`' ;


    bool getSettings(const std::string &path_to_ini) override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
    bool start_cam();
    // =========================================
    void exec();
    bool getNextFrame(cv::Mat& frame);
    void runHandleFrame();
}; // -- END class WebCamera : public Device
#endif // WEBCAMERA_HPP
