#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include "udp_modules/async_udp_server/async_udp_server.hpp"
#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "command_processing/commands_zoo/telemetry_orientation/telemetry_orientation_eth_cmd.hpp"
#include "tools/sys_time_extract/sys_time_extract.hpp"

using namespace eth_cmd_processing;
using namespace syst_time_extract;

int main(int argc, char* argv[])
{
    AsyncUDPSrvCfg srv_cfg;
    srv_cfg.srv_ip = "192.168.0.9";
    srv_cfg.srv_port = 8888;
    srv_cfg.cmdManager.tir_net2host.set(ETH_CMD_ORIENTATION_TELEMETRY, 194);
    bool srv_ok = false;
    AsyncUDPServer srv(srv_cfg, srv_ok);
    if(!srv_ok)
    {
        std::cout << "ERROR: setup srv failed" << std::endl;
        return 1;
    }

    std::shared_ptr<EthCmdBase> cmd_tmp;
    std::shared_ptr<EthCmdOrientTelemetry> cmd_orient;
    OrientationData data_orient;
    DateTime time_stamp;

    while(true)
    {
        if(srv.pull_one(cmd_tmp))
        {
            if(cmd_tmp->get_id() == ETH_CMD_ORIENTATION_TELEMETRY)
            {
                cmd_orient = std::static_pointer_cast<EthCmdOrientTelemetry>(cmd_tmp);
                cmd_orient->get(data_orient);
                time_stamp = getCurrentDateTime();
                std::cout << "RCV_CMD: [" << time_stamp.to_str() << "]: \n";
                std::cout << "pitch = " << data_orient.pitch << "\n";
                std::cout << "roll = " << data_orient.roll << "\n";
                std::cout << "yaw = " << data_orient.yaw << "\n\n\n";
            }
            else
            {
                std::cout << "WARNING: failed recognize cmd type" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    std::cout << "END main" << std::endl;
    return 0;
} // -- END main
