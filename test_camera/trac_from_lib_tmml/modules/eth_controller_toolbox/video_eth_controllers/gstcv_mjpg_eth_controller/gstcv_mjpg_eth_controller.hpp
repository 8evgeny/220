#ifndef RTP_ETH_CONTROLLER_HPP
#define RTP_ETH_CONTROLLER_HPP

#include <string>
#include <memory>
#include <vector>
#include "eth_cntrl_common_types.hpp"
#include <opencv2/core/mat.hpp>
#include <chrono>
#include <opencv2/videoio.hpp>



namespace rtp_eth::controller_V1
{
    struct RTPEthCntSettings
    {
        std::string path2ini;
        std::string node_alias; //Client, Server, ClientFrame, ServerFrame, ServerCmd1, ClientCmd3 etc..
        std::string ini_section_name; // Наименование блока (секции) в ini-файле
        eth_common::CommunicationModes communication_mode; // SIMPLEX/DUPLEX?

        std::string src_ip_v4 = "127.0.0.1";
        int src_port = 8080;

        std::string dst_ip_v4 = "127.0.0.2";
        int dst_port = 8080;

        cv::Size txFrameSize = {1024, 600};
        int tx_fps = 2; // Максимальный fps передачи фреймов (не буферизация, а прореживание)

        cv::Size rxFrameSize = {1024, 600};
        int rx_max_buffers = 3 ;

        int gst_tx_max_size_buffers = 1 ; // размер буфера на отправку изображений

        int gst_rx_frame_rate = 30;
        int gst_rx_clock_rate = 90000;
        int gst_rx_payload = 26;
    }; // END -- EthCntSettingsBase

    class MJPGEthController
    {
        public:
            MJPGEthController(const RTPEthCntSettings &settings);
            bool launch();
            bool quit();
            bool send_frame(const cv::Mat& frame); // отправка фрейма
            bool get_frame(cv::Mat &receive_frame); // блокируется поток в ожидании прихода фрейма

        private:
            MJPGEthController();
            bool configureTransmissionPipeline();
            bool launchTransmission();

            bool configureReceivePipeline();
            bool launchReceive();

            RTPEthCntSettings settings;

            cv::Mat frame_receive_raw;

            std::string gst_TX; // transmission pipeline
            std::string gst_RX; // receive pipeline

            bool execute_rx = false;
            bool execute_tx = false;

            cv::VideoCapture cap_receive;
            cv::VideoWriter writer_transmit;

            std::chrono::system_clock::time_point tp_send_frame;
            int min_send_frame_delay_ms;
    }; // class MJPGEthController

} // namespace rtp_eth::controller


#endif //RTP_ETH_CONTROLLER_HPP
