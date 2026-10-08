#include "udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.hpp"
#include "command_processing/commands_zoo/ap_pos_cnt/ap_pos_cnt_eth_cmd.hpp"
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>

using namespace eth_cmd_processing;

int main(int argc, char* argv[])
{
    std::shared_ptr<eth_controller::cmd::async::udp::AsyncUDPEthControllerCmd> udp_cntr
        = eth_controller::cmd::async::udp::create(
            "../modules/Eth_Controller_Toolbox/tests/manual/test_snd_ap_pos_cnt/settings.ini", 
            "udp_controller");
    if(udp_cntr == nullptr)
    {
        return 1;
    }

    auto mn_st = udp_cntr->get_minor_settings();
    mn_st.cmd_manager_server.tir_host2net.set(ETH_CMD_AP_POS_CNT, 22);
    mn_st.cmd_manager_server.checksum_codec_type = CHECKSUM_CODEC_PASS;
    mn_st.cmd_manager_server.cmd_id_codec_type = CMD_ID_CODEC_SIMPLE_DIRECT_ENC;
    
    if(!udp_cntr->set_minor_settings(mn_st))
    {
        return 1;
    }

    if(!udp_cntr->launch())
    {
        return 1;
    }

    size_t ctr = 0; // counter
    while(true)
    {
        // std::cout << "ctr = " << ctr << std::endl;
        std::shared_ptr<EthCmdApPosCnt> cmd_tmp = std::make_shared<EthCmdApPosCnt>();
        EthCmdDataApPosCnt cmd_data;
        cmd_data.button_id = (ctr++) % 255;
        cmd_tmp->set(cmd_data);
        std::cout << "button_id = " << (int)cmd_data.button_id << std::endl;
        if(!udp_cntr->send_cmd(cmd_tmp))
        {
            std::cout << "ERROR: send cmd failed" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    std::cout << "END main" << std::endl;
} // -- END main