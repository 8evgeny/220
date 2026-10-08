#ifndef RTSP_H265_DEVICE_SETTINGS_HPP
#define RTSP_H265_DEVICE_SETTINGS_HPP

#include <string>
#include <memory>

namespace devices::rtsp_h265_device
{
//    enum CameraModes
//    {
//        CAM_MODE_0 = 0,
//        CAM_MODE_3840_2160_6FPS = 1, // max birate = 16384
//        CAM_MODE_1280_720_20FPS = 2, // max birate = 8192
//        CAM_MODE_LAST = 3
//    }; // END CameraModes

    struct RtspsrcSettings
    {
        std::string ip = "192.168.1.64";
        std::string login = "admin";
        std::string pass = "hik12345";
        std::string codec = "h265";
        int port = 552;
        int channel = 1;
        int latency = 0;
    }; // END RtspsrcSettings

    struct AppsinkSettings
    {
        std::string name = "sink";
        std::string emit_signals = "true";
        std::string sync = "false";
        int max_buffers = 3;
        std::string drop = "true";
    }; // END AppsinkSettings

    struct VideoSettings // Запрашиваемые параметры видеопотока в gstreamer 
    {
        VideoSettings(int _width, int _height,int _fps) : width(_width), height(_height), fps(_fps){}
        VideoSettings(){}
        int width = 3840;
        int height = 2160;
        int fps = 6;
    }; // END VideoSettings

    struct OutputSettings
    {
        std::string format_out = "BGR";
        int square_mode = 0;
        int square_size = 2160;
        int fps = 1; // fps denominator
        int max_buffers = 3;// объем буффера gstreamer для складирования ожидающих выгрузки изображений 
    }; // END OutputSettings

    struct Settings
    {
        RtspsrcSettings src;
        AppsinkSettings sink;
        VideoSettings video;
        OutputSettings output;
    }; // END Settings

    using SettingsPtr = std::shared_ptr<Settings>;

} // END -- namespace devices::rtsp_h265_device

#endif //RTSP_H265_DEVICE_SETTINGS_HPP
