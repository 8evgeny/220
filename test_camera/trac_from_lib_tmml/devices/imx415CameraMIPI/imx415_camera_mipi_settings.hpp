#ifndef imx415_CAMERA_MIPI_SETTINGS_HPP
#define imx415_CAMERA_MIPI_SETTINGS_HPP

#include <string>
#include <memory>

namespace devices::imx415_camera_mipi
{
    enum CameraModes
    {
        CAM_MODE_1920_1080_15FPS,
        CAM_MODE_3840_2160_15FPS
    };

    struct UserSettings
    {
        int sensor_id = 0;
        int camera_mode =  0;
        int square_crop_sz = 1000;
        int fps_denominator = 1;
        int square_out_sz = 1080;
        int max_buffers = 3;
    };

    struct CameraSettings
    {
        int sensor_id = 0; // идентификатор сенсора в сис
    };

    struct VideoSettings // Запрашиваемые параметры видеопотока в gstreamer 
    {
        VideoSettings(
            int _width, 
            int _height, 
            int _fps) :
        width(_width),
        height(_height),
        fps(_fps)        
        {

        }

        VideoSettings()
        {

        }

        int width = 4032; 
        int height = 3040; 
        int fps = 20; 
    };

    struct OutputSettings
    {
        int fps = 1; // fps denominator
        int max_buffers = 3;// объем буффера gstreamer для складирования ожидающих выгрузки изображений 
    };

    struct Settings
    {
        CameraSettings camera;
        VideoSettings video;
        OutputSettings output;
    };
} // END -- devices::imx415_camera_mipi

#endif //imx415_CAMERA_MIPI_SETTINGS_HPP
