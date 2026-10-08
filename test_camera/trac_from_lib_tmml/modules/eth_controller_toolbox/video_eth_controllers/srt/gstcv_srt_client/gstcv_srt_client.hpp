#ifndef GSTCV_SRT_CLIENT_HPP
#define GSTCV_SRT_CLIENT_HPP

#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <atomic>

namespace gstcv
{
    struct SRTClientCfg
    {
        std::string srv_uri = "srt://127.0.0.1:8888";
        int max_buffers = 3;
    }; // -- struct SRTClientCfg

    class SRTClient
    {
    public:
        SRTClient(const SRTClientCfg &_cfg, bool &ok);
        bool getFrame(cv::Mat &frame_rcv);
        void quit();
        ~SRTClient();
    private:
        SRTClientCfg cfg;
        cv::VideoCapture cap;
        struct Sync
        {
            std::atomic<bool> quit_was_call = {false};
        } sync;
    }; // -- class SRTClient
} // -- namespace gstcv



#endif /* GSTCV_SRT_CLIENT_HPP */