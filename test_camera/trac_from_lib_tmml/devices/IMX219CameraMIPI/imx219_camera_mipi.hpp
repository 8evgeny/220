#ifndef IMX219_CAMERA_MIPI_HPP
#define IMX219_CAMERA_MIPI_HPP

#include "devices/device.hpp"
#include "camera_settings.hpp"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <atomic>
#include <condition_variable>
#include <memory>

namespace devices
{
    class IMX219CameraMIPI : public Device
    {
    public:
        IMX219CameraMIPI(const imx219_camera_mipi::Settings &settings);
        ~IMX219CameraMIPI();
        void setup();
        void start();
        void quit();

        uint8_t* receiveFrame(int &w, int &h, int &id, int &num) override; // временно не используется
        void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override; 
        int getColorChannels() override;
        bool isBayerColorChannel() override;
        void keyHandler(uchar &key) override;
        void workflow() override;

    private:
        void exec(); // прием фреймов из gst конвейера + конвертация формата
        void runHandleFrame(); // уведомление подписчиков (рассылка фремйма)

        /*
        Временной интервал (шаг) запроса фрейма из gst-конвейера. 
        Рассчитывается автоматически на основании ini-параметра "fps_output".
        При увеличении частоты запроса фреймов увеличивается ресурсоемкость процесса поставки фреймов application'у, 
        следовательно, снижается кадровая частота основного цикла приложения (fps_scan);
        */ 
        int frame_request_delay_ms = 50; //Рассчитывается автоматически на базе fps_output

        /*
            need_io_resize -- Флаг состояния соответствия разрешений
                входного (из gst) и выходного (в application) изображений
        */
        bool need_io_resize = true; // need Input/Output resize

        imx219_camera_mipi::Settings settings;
        struct Syncronisation
        {
            std::condition_variable frameReadyCv;
            std::mutex frameMtx;
            std::atomic<bool> handleFrameExecute = false;
            std::atomic<bool> frame_ready = false;
        } sync; // примитивы внутренней синхронизации
        cv::Mat frame_receive; // фрейм, принимаемый напрямую из gst-конвейера
#if defined(CCM_8UC3)
        cv::Mat frame_intermediate; // промежуточный преобразованный фрейм
#endif
        cv::Mat frame_prepared; // подготовленный к отправке подписчикам фрейм
        std::string pipeline;
        cv::VideoCapture gst_cap;
        int frame_counter = 0;
    };
} // -- END namespace devices

#endif //IMX219_CAMERA_MIPI_HPP
