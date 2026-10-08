#ifndef TCP_SERVER_SIMPLEX_FRAME_HPP
#define TCP_SERVER_SIMPLEX_FRAME_HPP

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include <atomic>
#include "../tcp_server_simplex_base.hpp"

namespace tcp_server_simplex::frame
{
    struct RXTX_FrameSettings
    {
        cv::Size receive_frame_size = {128, 128}; // {width, height} - размер отправляемого изображения на удаленный хост
        int buffer_size = 1; // Only for mode SAVE_BUFFER
    }; // -- END RXTX_FrameSettings



    class TCPServerSimplexFrame : public tcp_server_simplex::base::TCPServerSimplexBase
    {
    public:
        TCPServerSimplexFrame(
            RXTX_FrameSettings rxtx_settings, 
            tcp_server_simplex::base::SettingsBase settings_base);
        TCPServerSimplexFrame();
        ~TCPServerSimplexFrame();

        bool exec() override;
        bool quit() override; // Корректное завершение сервера, в т.ч. закрытие сокета

        bool wait_receive_frame(cv::Mat &received_frame) const; // ожидание прихода фрейма
        // bool isFrameReady() const; // возвращает статус готовности принятого фрейма

    private:
        RXTX_FrameSettings rxtx_settings;
        bool frameReadyStatus = false;

        //frame receive process variables

        cv::Mat frame_receive; // рабочий принимаемый фрейм
        uint8_t* frame_ptr; 
        const int preambule_len = 32;
        int package_buffer_len;
        uint8_t *frame_buffer;
        int rc_frame_data_row_len;

    }; // -- END class TCPServerSimplexFrame

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_FrameSettings &rxtx_settings);

    void checkValidIniSettings(
        const RXTX_FrameSettings &rxtx_settings);

    std::shared_ptr<TCPServerSimplexFrame> create(
        const std::string &path2ini,
        const std::string &sectionName);
} // -- END namespace tcp_server_simplex::base

#endif //TCP_SERVER_SIMPLEX_HPP