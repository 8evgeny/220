#ifndef IMX219_CAMERA_MIPI_SETTINGS_HPP
#define IMX219_CAMERA_MIPI_SETTINGS_HPP

#include <string>
#include <memory>

namespace devices::imx219_camera_mipi
{
    struct CameraSettings
    {
        int sensor_id = 0; // идентификатор сенсора в сис
        int wbmode = 1; // Эффекты баланса белого ref: https://developer.ridgerun.com/wiki/index.php/NVIDIA_Jetson_ISP_Control 
        int max_buffers = 3;// объем буффера gstreamer для складирования ожидающих выгрузки изображений
        bool enable_auto_exposure_range = false; // флаг состояния активности автоматической подстройки экспозиции
        std::pair<int, int> manual_exposure_range_ns = {0,0}; // пользовательский установленный (ini-файл) диапазон экспозиции
        const std::pair<int,int> limited_valid_exposure_range_ns = {34000, 358733000}; // только для imx219   
    };

    struct VideoSettings // Запрашиваемые параметры видеопотока в gstreamer 
    {
        int width = 3264; 
        int height = 2464; 
        int fps = 21; 
        int flip_method = 0; // поворот изображения 
    };
    
    struct OutputSettings // возвращаемые параметры видеопотока из device в application
    {
        int width = 3264;
        int height = 2464;
        int fps = 10;
    };

    struct Settings
    {
        CameraSettings camera;
        VideoSettings video;
        OutputSettings output;
    };
} // END -- devices::imx219_camera_mipi

#endif //IMX219_CAMERA_MIPI_SETTINGS_HPP