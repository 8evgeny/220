#ifndef MAT2RTSP_HPP
    #define MAT2RTSP_HPP

    #include "memory"
    #include "iostream"
    #include "INIReader.h"
    #include "thread"
    #include <opencv2/videoio.hpp>

    class mat2rtsp
    {
    public:
        mat2rtsp(const std::string &pathToConfig, bool &ok, int &frame_w, int &frame_h, int &frame_fps);
        ~mat2rtsp();
        mat2rtsp(const mat2rtsp &) = delete;
        mat2rtsp(mat2rtsp &&) = delete;
        mat2rtsp& operator=(mat2rtsp &&) = delete;
        void sendToRTSPServer(cv::Mat & );

    private:
        bool get_ini_params(const std::string &);
        std::unique_ptr<cv::VideoWriter> mat2rtsp_sender = nullptr;
        std::string port_mountpoint = "";
        std::string codec = "264";
        int bps = 0;
        int work = 0;
        int gop = -1;
// #ifdef USE_RTP
        int mtu = 1400;
        int config_interval = 5;
        std::string send_ip = "192.168.1.121";
        int port = 8081;
// #endif // USE_RTP

    }; // END class mat2rtsp
#endif // MAT2RTSP_HPP
