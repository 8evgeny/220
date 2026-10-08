#include "cmd_vertelka_keeper.hpp"

#include <iomanip>

namespace cmd_vertelka
{

CmdVertelkaKeeper::CmdVertelkaKeeper(const Settings &_settings) : settings(_settings)
{

} // -- END constructor


bool CmdVertelkaKeeper::setup()
{
    eth_cmd_distributor = std::make_shared<eth_cmd_processing::CmdHandlersDistributor>();
    if(eth_cmd_distributor == nullptr)
    {
        std::cout << "ERROR: create eth_cmd_distributor failed" << std::endl;
        return false;
    }

    udp_controller = eth_controller::cmd::async::udp::create(settings.path2ini, settings.section_name);
    std::cout << "DBG:: setup\nsettings.path2ini = " << settings.path2ini <<
                 "\nsettings.section_name = " << settings.section_name << std::endl;
    if(udp_controller == nullptr)
    {
        std::cout << "ERROR: create udp_controller failed" << std::endl;
        return false;
    }

    eth_controller::cmd::async::udp::MinorSettings controller_minor_settings;
    controller_minor_settings.cmd_manager_server.checksum_codec_type    = CHECKSUM_CODEC_PASS;
    controller_minor_settings.cmd_manager_server.cmd_id_codec_type      = CMD_ID_CODEC_SIMPLE_DIRECT_ENC;
    controller_minor_settings.cmd_manager_server.pass_decode_cmd_type   = ETH_CMD_NO_COMMAND;

    controller_minor_settings.cmd_manager_server.tir_net2host.set(ETH_CMD_VERTELKA_CLOSE, 0xAA);
    controller_minor_settings.cmd_manager_server.tir_net2host.set(ETH_CMD_VERTELKA_RUN8, 0xBB);
    controller_minor_settings.cmd_manager_server.tir_net2host.set(ETH_CMD_VERTELKA_RUN10, 0xCC);

    if(!udp_controller->set_minor_settings(controller_minor_settings))
    {
        std::cout << "ERROR: udp_controller->set_minor_settings" << std::endl;
        return false;
    }

    if(!udp_controller->launch())
    {
        std::cout << "ERROR: launch udp_controller failed" << std::endl;
        return false;
    }

    eth_cmd_distributor->register_cmd_handler(
                ETH_CMD_VERTELKA_CLOSE,
                std::bind(&CmdVertelkaKeeper::handleEthVertelkaClose, this, std::placeholders::_1));
    eth_cmd_distributor->register_cmd_handler(
                ETH_CMD_VERTELKA_RUN8,
                std::bind(&CmdVertelkaKeeper::handleEthVertelkaRun8, this, std::placeholders::_1));
    eth_cmd_distributor->register_cmd_handler(
                ETH_CMD_VERTELKA_RUN10,
                std::bind(&CmdVertelkaKeeper::handleEthVertelkaRun10, this, std::placeholders::_1));
    register_cmd_handler(std::bind(&CmdVertelkaKeeper::handleEthCmdDistributionGeneral, this, std::placeholders::_1));
    udp_controller->register_cmd_handler(this);
    // udp_controller->register_notify_handler(this);

    return true;
} // -- END setup

bool CmdVertelkaKeeper::check_valid()
{
    sync.tele_mtx.lock();
    bool is_valid = false;
    tp_tmp = std::chrono::high_resolution_clock::now();
    delay_enter_cmd_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp_tmp - tp_last_cmd).count();
    sync.tele_mtx.unlock();
    return is_valid;
} // -- END check_valid

void CmdVertelkaKeeper::get(CmdVertelka_list &cmd_vertelka_list_)
{
    sync.tele_mtx.lock();
//    std::cout << "CmdVertelkaKeeper::get::cmd_vertelka_list = " << cmd_vertelka_list.close_f << "; " << cmd_vertelka_list.run8_f << "; " << cmd_vertelka_list.run10_f << std::endl;
    cmd_vertelka_list_ = cmd_vertelka_list;
    cmd_vertelka_list = CmdVertelka_list();
//    cmd_vertelka_list = CmdVertelka_list(); // считав комманду, "обнуляем" структуру комманд, чтобы не вывполнять её дважды
    sync.tele_mtx.unlock();
} // -- END get




bool CmdVertelkaKeeper::quit()
{
    if(sync.quit_was_call.load())
    {
        return false;
    }
    sync.quit_was_call.store(true);

    if(!udp_controller->quit())
    {
        std::cout << "ERROR: udp_controller quit failed" << std::endl;
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!sync.tele_mtx.try_lock())
    {
        std::cout << "ERROR: mutex is lock when quit from CmdVertelkaKeeper" << std::endl;
    }
    sync.tele_mtx.unlock();

    return true;
} // -- END quit

void CmdVertelkaKeeper::handleEthCmdDistributionGeneral(std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd)
{
    eth_cmd_distributor->apply(receive_cmd);
} // -- END distribute gen


void CmdVertelkaKeeper::handleEthVertelkaClose(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd)
{
    std::cout << "DBG:: CmdVertelkaKeeper -- handleEthVertelkaClose" << std::endl;
    std::shared_ptr<eth_cmd_processing::EthCmdVertelka_close> cmd =
            std::dynamic_pointer_cast<eth_cmd_processing::EthCmdVertelka_close>(_receive_cmd);

    sync.tele_mtx.lock();
    cmd->get(cmd_vertelka_list.close);
    cmd_vertelka_list.close_f = 1;
    static std::chrono::system_clock::time_point tp_cur;
    tp_cur = std::chrono::system_clock::now();
    tp_last_cmd = tp_cur;
    sync.tele_mtx.unlock();

} // -- END handle APTelemetryCmd

void CmdVertelkaKeeper::handleEthVertelkaRun10(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd)
{
    std::cout << "DBG:: CmdVertelkaKeeper -- handleEthVertelkaRun10" << std::endl;
    std::shared_ptr<eth_cmd_processing::EthCmdVertelka_run_10> cmd =
            std::dynamic_pointer_cast<eth_cmd_processing::EthCmdVertelka_run_10>(_receive_cmd);

    sync.tele_mtx.lock();
    cmd->get(cmd_vertelka_list.run10);
    cmd_vertelka_list.run10_f = 1;
    static std::chrono::system_clock::time_point tp_cur;
    tp_cur = std::chrono::system_clock::now();
    tp_last_cmd = tp_cur;
    sync.tele_mtx.unlock();

} // -- END handle APTelemetryCmd

void CmdVertelkaKeeper::handleEthVertelkaRun8(std::shared_ptr<eth_cmd_processing::EthCmdBase> _receive_cmd)
{
    std::cout << "DBG:: CmdVertelkaKeeper -- handleEthVertelkaRun8" << std::endl;
    std::shared_ptr<eth_cmd_processing::EthCmdVertelka_run_8> cmd =
            std::dynamic_pointer_cast<eth_cmd_processing::EthCmdVertelka_run_8>(_receive_cmd);

    sync.tele_mtx.lock();
    cmd->get(cmd_vertelka_list.run8);
    cmd_vertelka_list.run8_f = 1;
    static std::chrono::system_clock::time_point tp_cur;
    tp_cur = std::chrono::system_clock::now();
    tp_last_cmd = tp_cur;
    sync.tele_mtx.unlock();

} // -- END handle APTelemetryCmd

} // -- namespace cmd_vertelka
