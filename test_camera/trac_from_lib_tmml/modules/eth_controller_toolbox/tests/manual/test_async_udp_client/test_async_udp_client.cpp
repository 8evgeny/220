#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "udp_modules/async_udp_client/async_udp_client.hpp"
#include "command_processing/commands_zoo/telemetry_orientation/telemetry_orientation_eth_cmd.hpp"
#include "tools/sys_time_extract/sys_time_extract.hpp"

using namespace syst_time_extract;
using namespace eth_cmd_processing;

int main(int argc, char* argv[])
{
    AsyncUDPCliCfg cli_cfg;
    cli_cfg.dst_ip = "127.0.0.1";
    cli_cfg.dst_port = 8888;
    cli_cfg.cmdManager.tir_host2net.set(ETH_CMD_ORIENTATION_TELEMETRY, 194);
    bool cli_ok = false;
    AsyncUDPClient cli(cli_cfg, cli_ok);
    if(!cli_ok)
    {
        std::cout << "ERROR: setup client failed" << std::endl;
        return 1;
    }

    DateTime time_stamp;
    size_t counter = 0;
    while(true)
    {
        std::shared_ptr<EthCmdOrientTelemetry> cmd 
            = std::make_shared<EthCmdOrientTelemetry>();
        OrientationData orient_data;
        orient_data.pitch = counter % 50;
        orient_data.roll = counter % 100;
        orient_data.yaw = counter % 150;

        time_stamp = getCurrentDateTime();
        std::cout << "SND: [" << time_stamp.to_str() << "]: \n";
        std::cout << "pitch = " << orient_data.pitch << "\n";
        std::cout << "roll = " << orient_data.roll << "\n";
        std::cout << "yaw = " << orient_data.yaw << "\n\n\n";

        cmd->set(orient_data);
        if(!cli.send_cmd(cmd))
        {
            std::cout << "ERROR: AsyncUDPClient: send cmd failed" << std::endl;
            break;
        }
        ++counter;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

    cli.quit();
    std::cout << "END main" << std::endl;
    return 0;
} // -- END main