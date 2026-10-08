#ifndef RASPBERRYHQCAMERA_SETTINGS_HPP
#define RASPBERRYHQCAMERA_SETTINGS_HPP

#include <string>
#include <memory>

namespace devices::raspberry_hq_camera
{
struct CameraSettings
{
    int sensor_id = 0;
    int bufapi_version = 1;
    int wbmode = 1;
    int sensor_mode = 1;
};

struct VideoSettings
{
    std::string format = "NV12";
    int width = 1920;
    int height = 1080;
    int fps = 30;
};

struct OutputSettings
{
    std::string format = "GRAY8";
    int width = 1920;
    int height = 1080;
};

struct Settings
{
    CameraSettings camera;
    VideoSettings video;
    OutputSettings output;
};

using SettingsPtr = std::shared_ptr<Settings>;
}

#endif // RASPBERRYHQCAMERA_SETTINGS_HPP
