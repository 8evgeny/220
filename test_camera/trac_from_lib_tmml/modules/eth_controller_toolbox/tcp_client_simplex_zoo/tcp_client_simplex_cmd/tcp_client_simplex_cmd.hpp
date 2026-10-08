#ifndef TCP_CLIENT_SIMPLEX_CMD_HPP
#define TCP_CLIENT_SIMPLEX_CMD_HPP

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

#include "../../command_processing/commands_zoo/eth_cmd_base.hpp"
#include "notification_handler/iethnotifyhandler.hpp"
#include "../tcp_client_simplex_base.hpp"

namespace tcp_client_simplex::cmd
{
    struct RXTX_CmdSettings
    {
        int single_command_buf_size_byte = 128; // размер буфера, выделяемый для приема команды (общий)
        int cmd_buffer_size_total = 1 ; // Размер буфера команд при отложенной отправке и неблокирующем вызове метода send_cmd
    }; // -- struct RXTX_CmdSettings

    class TCPClientSimplexCmd : public tcp_client_simplex::base::TCPClientSimplexBase
    {
    public:
        TCPClientSimplexCmd(
            tcp_client_simplex::base::NetworkSettings _network_settings, 
            tcp_client_simplex::cmd::RXTX_CmdSettings _rxtx_frame_settings);
        TCPClientSimplexCmd();
        ~TCPClientSimplexCmd() override;
        
        bool send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> eth_cmd); // поток блокируется вызовом, команда будет отправлена первым приоритетом (приоритет над регулярной отправкой буферных команд) 
        // bool buf_push_back_cmd(std::shared_ptr<eth_controller::cmd::EthCmdBase> eth_cmd); // команда добавляется в буфер регулярной отправки (вызов неблокирующий)
        bool quit() override;
        void register_notify_handler(IEthNotifyHandler *handler); // добавления подписчика на уведомления по поводу eth-взаимодействия
    private:
        RXTX_CmdSettings rxtx_cmd_settings;
        uint8_t* cmd_bytebuffer;
        std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- class TCPClientSimplexCmd

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_CmdSettings &settings);
    bool checkValidIniSettings(const RXTX_CmdSettings &settings);
    std::shared_ptr<TCPClientSimplexCmd> create(
        const std::string &path2ini,
        const std::string &sectionName);
} // -- namespace tcp_client_simplex::cmd

#endif //TCP_CLIENT_SIMPLEX_HPP
