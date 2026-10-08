#include "cmd400_keeper.hpp"

#include <iomanip>

namespace cmd400_keeper
{

Cmd400Keeper::Cmd400Keeper(const Settings &_settings) : settings(_settings)
{

} // -- END constructor


bool Cmd400Keeper::setup()
{
    eth_cmd_distributor = std::make_shared<eth_cmd_processing::CmdHandlersDistributor>();
    if(eth_cmd_distributor == nullptr)
    {
        std::cout << "ERROR: create eth_cmd_distributor failed" << std::endl;
        return false;
    } // END if(eth_cmd_distributor == nullptr)

    udp_controller = eth_controller::cmd::async::udp::create(settings.path2ini, settings.section_name);
    if(udp_controller == nullptr)
    {
        std::cout << "ERROR: create udp_controller failed" << std::endl;
        return false;
    } // END if(udp_controller == nullptr)

    eth_controller::cmd::async::udp::MinorSettings controller_minor_settings;
    controller_minor_settings.cmd_manager_server.checksum_codec_type    = CHECKSUM_CODEC_PASS;
    controller_minor_settings.cmd_manager_server.cmd_id_codec_type      = CMD_ID_CODEC_SIMPLE_DIRECT_ENC;
    controller_minor_settings.cmd_manager_server.pass_decode_cmd_type   = ETH_CMD_400_INIT;

    controller_minor_settings.cmd_manager_server.tir_net2host.set(ETH_CMD_400_INIT, 0x0A);
    controller_minor_settings.cmd_manager_server.tir_net2host.set(ETH_CMD_400_DEINIT, 0x0B);
    controller_minor_settings.cmd_manager_server.tir_net2host.set(ETH_CMD_400_RECORD_ON, 0x0C);


    if(!udp_controller->set_minor_settings(controller_minor_settings))
    {
        std::cout << "ERROR: udp_controller->set_minor_settings" << std::endl;
        return false;
    } // END  if(!udp_controller->set_minor_settings(controller_minor_settings))

    if(!udp_controller->launch())
    {
        std::cout << "ERROR: launch udp_controller failed" << std::endl;
        return false;
    } // END if(!udp_controller->launch())

    eth_cmd_distributor->register_cmd_handler(
                ETH_CMD_400_INIT,
                std::bind(&Cmd400Keeper::handleEthCmd400Init, this, std::placeholders::_1));
    eth_cmd_distributor->register_cmd_handler(
                ETH_CMD_400_DEINIT,
                std::bind(&Cmd400Keeper::handleEthCmd400Deinit, this, std::placeholders::_1));
    eth_cmd_distributor->register_cmd_handler(
                ETH_CMD_400_RECORD_ON,
                std::bind(&Cmd400Keeper::handleEthCmd400RecordOn, this, std::placeholders::_1));
    register_cmd_handler(std::bind(&Cmd400Keeper::handleEthCmdDistributionGeneral, this, std::placeholders::_1));
    udp_controller->register_cmd_handler(this);
    // udp_controller->register_notify_handler(this);

    return true;
} // -- END setup

bool Cmd400Keeper::check_valid()
{
    sync.tele_mtx.lock();
    bool is_valid = false;
    tp_tmp = std::chrono::high_resolution_clock::now();
    delay_enter_cmd_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp_tmp - tp_last_cmd).count();
    sync.tele_mtx.unlock();
    return is_valid;
} // -- END check_valid

void Cmd400Keeper::get(Cmd400_list &cmd400_list_)
{
    sync.tele_mtx.lock();
    cmd400_list_ = cmd400_list;
    cmd400_list = Cmd400_list(); // считав комманду, "обнуляем" структуру комманд, чтобы не вывполнять её дважды
    sync.tele_mtx.unlock();
} // -- END get




bool Cmd400Keeper::quit()
{
    if(sync.quit_was_call.load())
    {
        return false;
    } // END if(sync.quit_was_call.load())
    sync.quit_was_call.store(true);

    if(!udp_controller->quit())
    {
        std::cout << "ERROR: udp_controller quit failed" << std::endl;
        return false;
    } // END if(!udp_controller->quit())

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!sync.tele_mtx.try_lock())
    {
        std::cout << "ERROR: mutex is lock when quit from Cmd400Keeper" << std::endl;
    } // END if(!sync.tele_mtx.try_lock())
    sync.tele_mtx.unlock();

    return true;
} // -- END quit

void Cmd400Keeper::handleEthCmdDistributionGeneral(std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd)
{
    eth_cmd_distributor->apply(receive_cmd);
} // -- END distribute gen

void Cmd400Keeper::handleEthCmd400Init(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd)
{
    std::cout << "DBG:: Cmd400Keeper -- receive telemetry package" << std::endl;
    std::shared_ptr<eth_cmd_processing::EthCmd400_init> cmd =
            std::dynamic_pointer_cast<eth_cmd_processing::EthCmd400_init>(_receive_cmd);
    
    sync.tele_mtx.lock();
    cmd->get(cmd400_list.init_cmd);
    cmd400_list.init_f = 1;
    static std::chrono::system_clock::time_point tp_cur;
    tp_cur = std::chrono::system_clock::now();
    tp_last_cmd = tp_cur;
    sync.tele_mtx.unlock();

} // -- END handle APTelemetryCmd

void Cmd400Keeper::handleEthCmd400Deinit(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd)
{
    // std::cout << "DBG:: Cmd400Keeper -- receive telemetry package" << std::endl;
    std::shared_ptr<eth_cmd_processing::EthCmd400_deinit> cmd =
            std::dynamic_pointer_cast<eth_cmd_processing::EthCmd400_deinit>(_receive_cmd);

    sync.tele_mtx.lock();
    cmd->get(cmd400_list.deinit_cmd);
    cmd400_list.deinit_f = 1;
    static std::chrono::system_clock::time_point tp_cur;
    tp_cur = std::chrono::system_clock::now();
    tp_last_cmd = tp_cur;
    sync.tele_mtx.unlock();

} // -- END handle APTelemetryCmd

void Cmd400Keeper::handleEthCmd400RecordOn(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd)
{
    std::cout << "DBG:: Cmd400Keeper -- receive telemetry package" << std::endl;
    std::shared_ptr<eth_cmd_processing::EthCmd400_record_on> cmd =
            std::dynamic_pointer_cast<eth_cmd_processing::EthCmd400_record_on>(_receive_cmd);

    sync.tele_mtx.lock();
    cmd->get(cmd400_list.record_cmd);
    cmd400_list.record_switch_f = 1;
    static std::chrono::system_clock::time_point tp_cur;
    tp_cur = std::chrono::system_clock::now();
    tp_last_cmd = tp_cur;
    sync.tele_mtx.unlock();

} // -- END handle APTelemetryCmd

} // -- namespace cmd400_keeper
