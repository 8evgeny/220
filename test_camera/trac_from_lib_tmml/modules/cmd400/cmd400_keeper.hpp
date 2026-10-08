#ifndef CMD400_KEEPER_HPP
#define CMD400_KEEPER_HPP

// std
#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

#include "cmd400_struct.hpp"

// eth
#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "command_processing/commands_zoo/cmd400/eth_cmd_400_init.hpp"
#include "command_processing/commands_zoo/cmd400/eth_cmd_400_deinit.hpp"
#include "command_processing/commands_zoo/cmd400/eth_cmd_400_record_on.hpp"
#include "command_processing/icmdhandler.hpp"
#include "notification_handler/iethnotifyhandler.hpp"
#include "command_processing/cmd_handlers_distributor/cmd_handlers_distributor.hpp"
#include "udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.hpp"

// for log
//#include "tools/sys_time_toolbox/sys_time_extract.hpp"

namespace cmd400_keeper
{

struct Settings
{
    int max_delay_btw_packs_ms  = 1'500;
    std::string path2ini        = "../settings.ini";
    std::string section_name    = "cmd400_keeper";
};

struct Cmd400_list
{
    bool init_f = 0;
    bool deinit_f = 0;
    bool record_switch_f = 0;
    Cmd400_init init_cmd = {-1,-1,-1,-1};
    Cmd400_deinit deinit_cmd = {0};
    Cmd400_record_on record_cmd = {0};
};

class Cmd400Keeper : // AutoPilot Telemetry Keeper
   public ICmdHandler // обработка поступающих eth-команд
   , public IEthNotifyHandler // обработка поступающих eth-уведомлений
{
public:

    Cmd400_list cmd400_list;

    Cmd400Keeper(const Settings &_settings);
    bool setup();
    bool check_valid();
    void get(Cmd400_list &cmd400_list_);
    bool quit();
private:
    Cmd400Keeper() = delete;
    void handleEthCmdDistributionGeneral(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd); // общий маршрутизатор приходящего потока команд по конкретным обработчикам
    void handleEthCmd400Init(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd);
    void handleEthCmd400Deinit(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd);
    void handleEthCmd400RecordOn(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd);
    std::shared_ptr<eth_cmd_processing::CmdHandlersDistributor> eth_cmd_distributor = nullptr;

//    syst_time_extract::DateTime sys_datetime;
    int float_prec = 5;

    Settings settings;
    std::shared_ptr<eth_controller::cmd::async::udp::AsyncUDPEthControllerCmd> udp_controller = nullptr;
    struct Sync
    {
        std::mutex tele_mtx;
        std::atomic<bool> quit_was_call = {false};
        std::atomic<bool> success_quit = {false};
    } sync;

    std::chrono::system_clock::time_point   tp_last_cmd, // временная метка последнего поступившего пакета телеметрии
                                            tp_tmp; 
    int64_t delay_enter_cmd_ms;

}; // -- class Cmd400Keeper

} // -- namespace cmd400_keeper

#endif // CMD400_KEEPER_HPP
