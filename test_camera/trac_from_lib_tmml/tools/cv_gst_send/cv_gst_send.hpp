#ifndef CV_GST_SEND_HPP
    #define CV_GST_SEND_HPP
    #include "memory"
    #include <opencv2/videoio.hpp>

    class cv_gst_send
    {
    public:
        cv_gst_send(const std::string &pathToConfig, bool &ok, int &frame_w, int &frame_h, int &frame_fps);
        ~cv_gst_send();
        cv_gst_send(const cv_gst_send &) = delete;
        cv_gst_send(cv_gst_send &&) = delete;
        cv_gst_send& operator=(cv_gst_send &&) = delete;
        void sendToHost(cv::Mat & );

    private:
        bool get_ini_params(const std::string &);
        std::unique_ptr<cv::VideoWriter> cv_gst_sender = nullptr;
        std::string pipeline1 = "";
        std::string pipeline2 = "";
        std::string pipeline3 = "";
        int send_frame_w = 1920;
        int send_frame_h = 1080;
        int fps = 30;
    }; // END class cv_gst_send
#endif // CV_GST_SEND_HPP
