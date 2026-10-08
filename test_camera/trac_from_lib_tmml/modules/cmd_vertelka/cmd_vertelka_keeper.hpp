#ifndef CMD_VERTELKA_KEEPER_HPP
#define CMD_VERTELKA_KEEPER_HPP

// std
#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

#include "cmd_vertelka_struct.hpp"

// eth
#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "command_processing/commands_zoo/cmd_vertelka/eth_cmd_vertelka_close.hpp"
#include "command_processing/commands_zoo/cmd_vertelka/eth_cmd_vertelka_run_10.hpp"
#include "command_processing/commands_zoo/cmd_vertelka/eth_cmd_vertelka_run_8.hpp"
#include "command_processing/icmdhandler.hpp"
#include "notification_handler/iethnotifyhandler.hpp"
#include "command_processing/cmd_handlers_distributor/cmd_handlers_distributor.hpp"
#include "udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.hpp"

// for log
//#include "tools/sys_time_toolbox/sys_time_extract.hpp"

namespace cmd_vertelka
{

struct Settings
{
    int max_delay_btw_packs_ms  = 1'500;
    std::string path2ini        = "../settings.ini";
    std::string section_name    = "cmd_vertlka_keeper";
};

struct CmdVertelka_list
{
    bool close_f = 0;
    bool run10_f = 0;
    bool run8_f = 0;

    CmdVertelka_close close = {0};
    CmdVertelka_run_8 run8 = {0};
    CmdVertelka_run_10 run10 = {0};
};

class CmdVertelkaKeeper : // AutoPilot Telemetry Keeper
   public ICmdHandler // обработка поступающих eth-команд
   , public IEthNotifyHandler // обработка поступающих eth-уведомлений
{
public:

    CmdVertelka_list cmd_vertelka_list;

    CmdVertelkaKeeper(const Settings &_settings);
    bool setup();
    bool check_valid();
    void get(CmdVertelka_list &cmd_vertelka_list_);
    bool quit();
private:
    CmdVertelkaKeeper() = delete;
    void handleEthCmdDistributionGeneral(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd); // общий маршрутизатор приходящего потока команд по конкретным обработчикам
    void handleEthVertelkaClose(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd);
    void handleEthVertelkaRun8(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd);
    void handleEthVertelkaRun10(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd);
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

}; // -- class CmdVertelkaKeeper

} // -- namespace cmd_vertelka

#endif // CMD_VERTELKA_KEEPER_HPP
