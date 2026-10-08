#ifndef TCP_CLIENT_SIMPLEX_FRAME_HPP
#define TCP_CLIENT_SIMPLEX_FRAME_HPP

#include <string>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>

#include "../tcp_client_simplex_base.hpp"

namespace tcp_client_simplex::frame
{
    struct RXTX_FrameSettings
    {
        cv::Size transmit_frame_size = {128, 128}; // {width, height} - размер отправляемого изображения на удаленный хост
    }; // -- END RXTX_FrameSettings

    class TCPClientSimplexFrame : public tcp_client_simplex::base::TCPClientSimplexBase
    {
    public:
        TCPClientSimplexFrame(
            const tcp_client_simplex::base::NetworkSettings &network_settings,
            const RXTX_FrameSettings &rxtx_frame_settings);
        ~TCPClientSimplexFrame() override;
        bool send_frame(const cv::Mat &frame, const cv::Point& obj_predict_danger);
    private:
        TCPClientSimplexFrame();
        RXTX_FrameSettings rxtx_frame_settings;
        cv::Mat frame_send;
    }; // -- END TCPClientSimplexFrame

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_FrameSettings &settings);
    bool checkValidIniSettings(const RXTX_FrameSettings &settings);
    std::shared_ptr<TCPClientSimplexFrame> create(
        const std::string &path2ini,
        const std::string &sectionName);

} // -- END namespace tcp_client_simplex::frame

#endif //TCP_CLIENT_SERVER_SIMPLEX_FRAME_HPP