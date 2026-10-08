#ifndef CORSAIR400RAW_HPP
#define CORSAIR400RAW_HPP


#include "../device.hpp"
#include "corsair400raw_settings.hpp"
#include "eth/servers/RAWVideoServer.hpp"

#include <opencv2/opencv.hpp>

#include <atomic>
#include <mutex>


namespace devices
{

class Corsair400raw : public Device
{
    public:
        Corsair400raw(const corsair_400_raw::Settings &_settings, bool &report);
        ~Corsair400raw();
        
        void setup() override; /* Запускает прием видео с камеры */
        void start() override; /* not use (but need implement interface) */
        void quit() override; /* Корректно завершает работы камеры*/

        uint8_t* receiveFrame(int &w, int &h, int &id, int &num) override; // временно не используется
        void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override; 
        // float getExposureMs() override;
        int getColorChannels() override;
        bool isBayerColorChannel() override;
        void keyHandler(unsigned char &key) override;
        void workflow() override;
        bool getState() override;
        // bool getState(ParitelTelemetryGen & state_frame) override;

    private:
        struct Sync
        {
            std::atomic<bool> execute = {false};
        } sync;
        
        virtual int getId() {return frame_id;}

        bool start_backend(); 
        Corsair400raw() = delete;
        void exec(); // цикл приема видео с камеры


        std::unique_ptr<RAWVideoServer> _rawRcv = nullptr;
        corsair_400_raw::Settings settings;
        cv::Size receive_frame_size;
        cv::Mat frame_receive;

        long frame_id = 0;
        long frame_id_cycle_reset = 10'000'000;
        cv::Size first_frame_sz = {-1,-1};
        int first_frame_ch_id = -1;
        int frame_len_bytes = -1;
        bool first_frame = true;
}; // class Corsair400raw

} // -- namespace devices
#endif // CORSAIR400RAW_HPP
