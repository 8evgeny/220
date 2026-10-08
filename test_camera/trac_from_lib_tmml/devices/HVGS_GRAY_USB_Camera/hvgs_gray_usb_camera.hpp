#ifndef HVGS_GRAY_USB_CAMERA_HPP
#define HVGS_GRAY_USB_CAMERA_HPP

#include "../device.hpp"

#include "hvgs_gray_usb_camera_settings.hpp"
#include "CameraApi.h"

#include <opencv2/opencv.hpp>

#include <atomic>
#include <mutex>

#include "tools/esrt_toolbox/esrt_api.hpp"
#include "tools/paritel_telemetry_multiplexor/paritel_telemetry_ap.hpp"
#include "tools/paritel_telemetry_multiplexor/paritel_telemetry_general.hpp"
#include "tools/paritel_telemetry_multiplexor/paritel_telemetry_multiplexor.hpp"


namespace devices
{

class HVGS_Gray_USB_Camera : public Device
{
    public:
        HVGS_Gray_USB_Camera(const hvgs_gray_usb_camera::Settings &_settings, bool &report);
        ~HVGS_Gray_USB_Camera();
        
        void setup() override; /* Запускает прием видео с камеры */
        void start() override; /* not use (but need implement interface) */
        void quit() override; /* Корректно завершает работы камеры*/

        uint8_t* receiveFrame(int &w, int &h, int &id, int &num) override; // временно не используется
        void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override; 
        float getExposureMs() override;
        int getColorChannels() override;
        bool isBayerColorChannel() override;
        void keyHandler(char &key) override;
        void workflow() override;
        bool getState(ParitelTelemetryGen & state_frame) override;

    private:

        virtual int getId() {return frame_id;}


        bool start_backend(); 
        HVGS_Gray_USB_Camera() = delete;
        void exec(); // цикл приема видео с камеры
        bool quit_d(); // прямой вызов основных функций деинициализации камеры

        hvgs_gray_usb_camera::Settings settings;
        cv::Size receive_frame_size;
        cv::Mat frame_receive;
        struct FrameMeta
        {
            float exposure_ms = -1; 
        } fr_meta;

        long frame_id = 0;

        // Синхронизация quit
        struct SyncQuit
        {
            std::atomic<bool> was_call      = {false};
            std::atomic<bool> success       = {false};
            std::atomic<bool> failed        = {false};
        };

        // Синхронизация exec
        struct SyncExec
        {
            std::atomic<bool> execute       = {false}; // исполняется в текущий момент
            std::atomic<bool> wait_close    = {false}; // execute установлен false, ожидается завершение цикла
            std::atomic<bool> close         = {false}; // завершен
        };

        // Общая синхронизация 
        struct Sync
        {
            SyncExec            exec;
            SyncQuit            quit;
        } sync;

        int                     hCamera;
        tSdkCameraCapbility     tCapability;
        tSdkImageResolution     img_res;
}; // class HVGS_Gray_USB_Camera

} // -- namespace devices
#endif // HVGS_GRAY_USB_Camera
