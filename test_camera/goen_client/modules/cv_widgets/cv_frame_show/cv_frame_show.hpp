#ifndef CV_FRAME_SHOW_HPP
#define CV_FRAME_SHOW_HPP


#include "iostream"

#include "tools/INIReader.h"
#include "devices/device.hpp"
#include "devices/iframehandler.h"
#include "cv_widget.hpp"


#ifdef USE_LOGGER
#include "logger/factory.h"
#endif //USE_LOGGER

#ifdef USE_FOLDER_READER
#include "devices/FolderReader/folderreader.h"
#endif // USE_FOLDER_READER

#ifdef USE_GST_PIPELINE_DEVICE
#include "devices/GstPipelineDevice/gst_pipeline_device_factory.hpp"
#endif // USE_GST_PIPELINE_DEVICE

#ifdef USE_RESEND_RTSP
#include "mat2rtsp.hpp"
#endif // USE_RESENDE_RTSP

class MainWindowConstructor;

namespace cvw
{

enum Devices
{
    FOLDER = 0,
    VIDEO = 1,
    WEBCAMERA = 2,
    COLIBRITV = 3,
    RASPBERRY_HQ_CAMERA = 4,
    IMX477_SQUARE_CAMERA_MIPI = 5,
    IMX219_CAMERA_MIPI = 6,
    RTSP_H265 = 7,
    SHARED_MEMORY = 8,
    HVGS_GRAY_USB_CAMERA = 9,
    GOEN_400_RAW = 10,
    GST_RTP = 11,
    GST_PIPELINE_DEVICE = 12
}; // -- END enum Devices

class CVFrameShow : public CVWidget, IFrameHandler
{
public:
    CVFrameShow();
    CVFrameShow(const std::string & config, bool & ok, std::string name_, CVMainWindow * main_window, cv::Rect2f rct_, std::function<void(MainWindowConstructor *, int, int)> func, MainWindowConstructor * mw_);
    void add_slot(std::function<void(MainWindowConstructor *, int x, int y)> func, MainWindowConstructor * mw_) {slot = func; mw_default = mw_;} // отклик на нажатие кнопки
    void show(cv::Mat & img); // END show
    bool start();
    bool stop();
    bool get_state();
    std::string parce_IP(const string & in);
    void SetChannelIR() {channel_id = 1;}
    void SetChannelTV() {channel_id = 0;}
    int getZahvatSize()
    {
        if(channel_id == 0) // TV
        {
            return zahvat_size_TV;
        } // END  if(channel_id == 0)
        if(channel_id == 1) // TPV
        {
            return zahvat_size_TPV;
        } // END if(channel_id == 1)
    }; // -- END getZahvatSize()

    void setZahvatSize(int sz_);
    void setZoomValue(float zoom_val_) {zoom_val = zoom_val_;};
    void setTracPos2Show(int pt_x, int pt_y);
    cv::Size get_frame_size();
    int device_id = 0; // в соответствии с enum Devices

    std::string location_rtsp = "location=";
    std::string location = "";
    std::string rtsp_url = "rtsp://192.168.144.101:554";
#ifdef USE_RESEND_RTSP
    std::shared_ptr<mat2rtsp> mat2rtsp_sender = nullptr;
#endif // USE_RESEND_RTSP
private:
    std::function<void(cv::Mat&)> func_show;
    cv::Point wh_2_zahvat_TPV = {9,9};
    cv::Point wh_2_zahvat_TV = {18,18};
    int zahvat_size_TPV = 18;
    int zahvat_size_TV = 36;
    float zoom_val = 1.f;
    bool f_close = true;
    std:: string name = "CVFrameShow";
    int mouse_status = -1;
    std::function<void(MainWindowConstructor *, int, int)> slot; // defaultSlot;
    MainWindowConstructor * mw_default = nullptr;
    cv::Rect rct_mouse = {0,0,0,0};
    bool f_draw_mouse_pos = false;
    std::string send_ip;
    int track_aim_x = 0;
    int track_aim_y = 0;
    std::string config_path = "../config.ini";
    // устройство считвания кадров
    std::shared_ptr<Device> device = nullptr;				// источник видеопотока
    // Временные точки для нахождения времени обработки кадра.
    std::chrono::system_clock::time_point time_point_device0, time_point_device1;
    // обработка изображения (трекинг)
    //    std::string location_rtsp = "location=";
    //    std::string location = "";
    //    std::string rtsp_url = "rtsp://192.168.144.101:554";

    bool show_win = false;
    std::string winname = "RTSP";
    int parallel = 1;
    int frame_w = 1920;
    int frame_h = 1080;
    float frame_w_1 = 1.f / frame_w;
    float frame_h_1 = 1.f / frame_h;
    std::atomic<bool> frameReady = false; // флаг готовности фрейма
    std::mutex frame_proc_0_mutex, frame_proc_1_mutex;
    cv::Mat frame_receive;
    cv::Mat frame_process_0; // двойная буфферизация
    cv::Mat frame_process_1;
    cv::Mat frame_show;                 // кадр для показа на экране

    std::atomic<bool> f_show_trac = {false};
    cv::Point pt_trac2show = cv::Point(0,0);


    int TV_w = 1920;
    int TV_h = 1080;
    float TV_w_1 = 1.f / TV_w;
    float TV_h_1 = 1.f / TV_h;
    int TPV_w = 640;
    int TPV_h = 512;
    int TPV_w_bord_2 = round(0.5 * TV_h * TPV_w / TPV_h);
    float TV_TPV_rel = 1080.f / 512.f;
    std::atomic<int> process_frame_id = 0; // номер (0,1) текущего обрабатываемого фрейма в главном цикле программы
    int channel_id = 0; // 0 - visible, 1 - infrared
    int trac_w_2 = 18;
    int trac_h_2 = 18;
    void mouse_handler(int & event, int x, int y);
    void handleDeviceFrame(uint8_t *f, int w, int h, int num, int id); // -- END handleDeviceFrame
    void next_frame_sync();
    void next_frame_async();
    bool get_ini_params(const std::string &pathToSettings, const std::string &ini_section_name);
}; // END class CVFrameShow

}; // END namespace cvw
#endif // CV_FRAME_SHOW_HPP


