#ifndef ETH_CONTROLLER_FRAME_HPP
#define ETH_CONTROLLER_FRAME_HPP

#include <opencv2/core/types.hpp>
#include <memory>
#include <chrono>
#include "../tcp_eth_controller_base.hpp"
#include "../../tcp_client_simplex_zoo/tcp_client_simplex_frame/tcp_client_simplex_frame.hpp"
#include "../../tcp_server_simplex_zoo/tcp_server_simplex_frame/tcp_server_simplex_frame.hpp"

namespace eth_controller::frame
{
    struct EthCntSettingsFrame // Transmit/Receive Frame Settings
    {
        int max_transmit_fps = 1 ; // приблизительная частота отправки фреймов при регулярном запросе метода send_frame(...)
    }; // -- END struct RXTX_FrameSettings

    class EthControllerFrame : public eth_controller::base::EthControllerBase
    {
        public:
            EthControllerFrame(const EthCntSettingsFrame &settings_frame, 
                const eth_controller::base::EthCntSettingsBase &settings_base,
                std::shared_ptr<tcp_server_simplex::frame::TCPServerSimplexFrame> simplex_server,
                std::shared_ptr<tcp_client_simplex::frame::TCPClientSimplexFrame> simplex_client);
            ~EthControllerFrame() override;
            bool launch() override;
            bool quit() override;
            bool send_frame(const cv::Mat& frame, const cv::Point &obj_predict_danger); // отправка фрейма
            bool wait_receive_frame(cv::Mat &receive_frame); // блокируется поток в ожидании прихода фрейма
        private:
            EthControllerFrame(); // Заблокированный конструктор по умолчанию
            EthCntSettingsFrame settings_frame;
            std::shared_ptr<tcp_server_simplex::frame::TCPServerSimplexFrame> server_simplex; 
            std::shared_ptr<tcp_client_simplex::frame::TCPClientSimplexFrame> client_simplex;
            std::chrono::system_clock::time_point tp_send_frame;
            int min_send_frame_delay_ms;
    }; // -- END class EthControllerFrame

    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        EthCntSettingsFrame &settings);
    bool checkValidIniSettings(const EthCntSettingsFrame &settings);
    std::shared_ptr<EthControllerFrame> create(const std::string &path2ini, const std::string &ini_section_name);

} // -- END namespace eth_controller

#endif // ETH_CONTROLLER_FRAME_HPP
