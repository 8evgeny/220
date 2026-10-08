#ifndef ETH_CONTROLLER_CMD_HPP
#define ETH_CONTROLLER_CMD_HPP

#include "../../tcp_client_simplex_zoo/tcp_client_simplex_cmd/tcp_client_simplex_cmd.hpp"
#include "../../tcp_server_simplex_zoo/tcp_server_simplex_cmd/tcp_server_simplex_cmd.hpp"
#include "../tcp_eth_controller_base.hpp"
#include "../../command_processing/icmdhandler.hpp"
#include "../../notification_handler/iethnotifyhandler.hpp"

// #include <vector>
// #include <memory>
// #include <atomic>

// #include "../command_processing/commands_zoo/eth_cmd_base.hpp"
// #include "../command_processing/commands_zoo/copter_alarm/copter_alarm_eth_cmd.hpp"

namespace eth_controller::cmd
{
    struct EthCntSettingsCmd
    {
        int max_transmit_cps = 1 ; //CPS - commands per second;
    }; // END -- EthCntSettingsCmd

    class EthControllerCmd : public eth_controller::base::EthControllerBase
    {
    public:
        EthControllerCmd(
            const EthCntSettingsCmd &settings_cmd,
            const eth_controller::base::EthCntSettingsBase settings_base,
            std::shared_ptr<tcp_client_simplex::cmd::TCPClientSimplexCmd> client_simplex,
            std::shared_ptr<tcp_server_simplex::cmd::TCPServerSimplexCmd> server_simplex);
        ~EthControllerCmd() override;
        bool launch() override; // открывает сокет и включает ожидание присоединения dst хоста (блокирует поток, из которого была вызвана до момента соединения с dst узлом)
        bool quit() override; // корректно завершает работу EthController
        
        bool send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> command) const; // отправка команды
        bool wait_receive_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd) const;

        void register_cmd_handler(ICmdHandler *handler);
        void register_notify_handler(IEthNotifyHandler *handler);
        // bool start_cmd_receive(); // начинает процесс прослушивания канала на предмет команд, не блокирует поток (открывает новый поток)
        // bool exec_cmd_receive(); // ??
        // int get_total_count_buffer_cmd(); // возвращает количество команд в буфере
        // bool cmd_buf_pop_back(); // забрать одну команду с конца буфера

    private:
        // void exec_cmd_receive(); // функция-поток прослушивания канала и складирования приходящих команд
        // bool wait_receive_cmd(eth_controller::cmd::EthCmdCopterAlarm &receive_cmd) const; // блокирует поток до момента приема команды
        
        std::shared_ptr<tcp_server_simplex::cmd::TCPServerSimplexCmd> server_simplex = nullptr;
        std::shared_ptr<tcp_client_simplex::cmd::TCPClientSimplexCmd> client_simplex = nullptr;
        // tcp_server_simplex::cmd::checkValidIniSettings();
        EthCntSettingsCmd settings_cmd;
        std::vector<IEthNotifyHandler *> notify_handlers;
        // struct CmdSync
        // {
        //     std::atomic<bool> cmd_receive_execute; // true, когда исполняется поток прослушивания канала на предмет приходящих команд
        //     std::mutex cmd_buffer_mtx; // обеспечение потоковой безопасности асинхронного чтения/записи буфера команд
        // } cmd_sync;

        // std::vector<std::shared_ptr<eth_controller::cmd::EthCmdBase>> cmd_buffer;
    }; // -- class EthController


    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        EthCntSettingsCmd &settings);
    bool checkValidIniSettings(const EthCntSettingsCmd &settings);
    std::shared_ptr<EthControllerCmd> create(const std::string &path2ini, const std::string &sectionName);
}

#endif //ETH_CONTROLLER_HPP
