#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <math.h>

#include "udp_modules/async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.hpp"
#include "command_processing/commands_zoo/ap_pos_cnt/ap_pos_cnt_eth_cmd.hpp"

std::string path2ini = "../modules/Eth_Controller_Toolbox/tests/manual/test_rcv_ap_pos_cnt/settings.ini";
std::string section = "udp_controller";

float rad2deg(float _rad)
{
    return 180 * _rad / M_PI;
}

class OrientTeleHandler : public ICmdHandler
{
public:
    OrientTeleHandler()
    {
        register_cmd_handler(std::bind(&OrientTeleHandler::cmd_handler, this, std::placeholders::_1));
    }
    void cmd_handler(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        std::shared_ptr<eth_cmd_processing::EthCmdApPosCnt> cmd_parsed 
            = std::static_pointer_cast<eth_cmd_processing::EthCmdApPosCnt>(cmd);
        eth_cmd_processing::EthCmdDataApPosCnt orient;
        cmd_parsed->get(orient);
        std::cout << (int)orient.button_id << std::endl;
    }
};

int main(int argc, char* argv[])
{
    auto udp_cnt = eth_controller::cmd::async::udp::create(path2ini, section);
    if(udp_cnt == nullptr)
    {
        return 1;
    }
    auto mn_st = udp_cnt->get_minor_settings();
    mn_st.cmd_manager_server.tir_net2host.set(ETH_CMD_AP_POS_CNT, 22);
    mn_st.cmd_manager_server.checksum_codec_type = CHECKSUM_CODEC_PASS;
    mn_st.cmd_manager_server.cmd_id_codec_type = CMD_ID_CODEC_SIMPLE_DIRECT_ENC;
    
    if(!udp_cnt->set_minor_settings(mn_st))
    {
        return 1;
    }

    OrientTeleHandler tele_handler;
    udp_cnt->register_cmd_handler(&tele_handler);

    if(!udp_cnt->launch())
    {
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::seconds(60));

    if(!udp_cnt->quit())
    {
        return false;
    }
    std::cout << "END main" << std::endl;
} // -- END main