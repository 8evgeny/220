#ifndef HVGS_GRAY_USB_CAM_SETTINGS_HPP
#define HVGS_GRAY_USB_CAM_SETTINGS_HPP

#include <string>

namespace devices::hvgs_gray_usb_camera
{
    struct CameraSettings
    {
        int id = 0;
        int fps = 43;
    };

    enum ExposureMode
    {
        EXPOSURE_MANUAL,
        EXPOSURE_AUTO
    };

    struct AutoExposureSettings
    {
        int brightness_target = 120;
    };

    struct ManualExposureSettings
    {
        int analog_gain = 4;
        float exposure_time_ms = 10;
    };

    struct LUTMappingSettings
    {
        int gamma = 100;
        int contrast = 100;
    };

    struct ImagePreprocessingSettings
    {
        LUTMappingSettings lut;
        int sharpness = 0 ;         /*
                                        sharpness устанавливает уровень резкости изображения. 
                                        Чем выше резкость, тем лучше четкость изображения, 
                                        но и больше шума; наоборот при снижении резкости изображения, 
                                        ощущение размытости сильное, но шум очень низкий. 
                                        Значение по умолчанию — 0, то есть эффект повышения резкости отсутствует.
                                    */
        bool mirror_vertical = false;
        bool mirror_horizontal = false;
    };

    struct OutputSettings
    {
        std::string format = "GRAY8";
        int width = 2592;
        int height = 2048;
    };

    struct WatchDogSettings
    {
        int read_frame_watchdog_ms          = 1000;     // предельное допустимое время ожидания нового кадра при вызове getFrame(frame)
        int check_frame_ready_delay_ms      = 2;        // интервал проверки готовности кадра
        int exec_complete_watchdog_ms       = 1'000;    // допустимое время ожидания завершения исполнения exec-цикла
        int check_exec_complete_delay_ms    = 15;       // интервал проверки на предмет завершения exec-цикла
    };

    struct Settings
    {
        std::string                 path2ini;
        std::string                 section_name;
        CameraSettings              camera;
        ExposureMode                exposure_mode = EXPOSURE_AUTO;
        AutoExposureSettings        auto_exposure;
        ManualExposureSettings      manual_exposure;
        ImagePreprocessingSettings  preprocess;
        OutputSettings              output;
        WatchDogSettings            watchdog;
    };

} // -- namespace devices::hvgs_gray_usb_camera

#endif // HVGS_GRAY_USB_CAM_SETTINGS_HPP
