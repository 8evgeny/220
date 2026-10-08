#ifndef GSTCV_SRT_SERVER_HPP
#define GSTCV_SRT_SERVER_HPP

#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <atomic>

namespace gstcv
{
    struct SRTServerCfg
    {
        std::string srv_uri = "srt://:8888";
        cv::Size frame_size = {1920, 1080};
        bool is_color = true;
        float fps = 25;
    }; // -- struct SRTServerCfg

    class SRTServer
    {
    public:
        SRTServer(const SRTServerCfg &_cfg, bool &ok);
        bool sendFrame(cv::Mat &frame_snd);
        void quit();
        ~SRTServer();
    private:
        SRTServerCfg cfg;
        cv::VideoWriter writer;
        struct Sync
        {
            std::atomic<bool> quit_was_call = {false};
        } sync;
    }; // -- class SRTServer
} // -- namespace gstcv


#endif // GSTCV_SRT_SERVER_HPP