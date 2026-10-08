#ifndef TCP_SERVER_SIMPLEX_CMD_HPP
#define TCP_SERVER_SIMPLEX_CMD_HPP

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include <atomic>
#include "../tcp_server_simplex_base.hpp"
#include "../../command_processing/commands_zoo/eth_cmd_base.hpp"

#include "../../command_processing/eth_cmd_classification.hpp"
#include "../../command_processing/icmdhandler.hpp"
#include "notification_handler/iethnotifyhandler.hpp"

namespace tcp_server_simplex::cmd
{
    struct RXTX_CmdSettings
    {
        unsigned int single_command_buf_size_byte = 128; // размер буфера на прием одной изолированной команды в байтах
    };

    class TCPServerSimplexCmd : public tcp_server_simplex::base::TCPServerSimplexBase
    {
    public:
        TCPServerSimplexCmd(RXTX_CmdSettings settings, tcp_server_simplex::base::SettingsBase base_settings);
        TCPServerSimplexCmd();
        ~TCPServerSimplexCmd();
        
        bool exec() override; 
        bool quit() override; // корректное завершение сервера, в т.ч. закрытие сокета
        
        void register_cmd_handler(ICmdHandler *handler);

        // eth notify block
        void register_notify_handler(IEthNotifyHandler *handler);
        void apply_notify_all_handlers(eth_notify_utils::EthNotifyType notify_type); // Отправка уведомления всем хендлерам

    private:
        bool cmd_ready_status = false;
        RXTX_CmdSettings rxtx_settings;
        uint8_t* cmd_bytebuffer;
        eth_cmd_processing::EthCmdDemux eth_cmd_demux;
        std::vector<ICmdHandler *> cmd_handlers;
        std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- class TCPServerSimplexCmd

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_CmdSettings &settings);

    bool checkValidIniSettings(
        const RXTX_CmdSettings &settings);

    std::shared_ptr<TCPServerSimplexCmd> create(
        const std::string &path2ini, 
        const std::string &sectionName);
} // -- END namespace tcp_server_simplex::base

#endif //TCP_SERVER_SIMPLEX_HPP