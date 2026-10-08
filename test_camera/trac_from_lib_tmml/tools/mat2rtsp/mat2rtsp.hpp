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
        void kill_system_process(const std::string &);
        void get_system_result(const std::string& system_str, std::string& cmd_result);
        std::unique_ptr<cv::VideoWriter> mat2rtsp_sender = nullptr;
        int port = 0;
        std::string mountpoint = "imx415";
        std::string codec = "264";
        int bps = 0;
        int work = 0;


 #if defined(CCM_8UC1)
        bool isColor = 0;
 #elif defined(CCM_8UC3)
        bool isColor = 1;
 #endif // END defined(CCM_8UC1)
    }; // END class mat2rtsp
#endif // MAT2RTSP_HPP
