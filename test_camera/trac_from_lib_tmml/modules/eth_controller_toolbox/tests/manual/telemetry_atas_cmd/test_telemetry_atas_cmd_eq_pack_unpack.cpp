#include <iostream>
#include "command_processing/commands_zoo/telemetry_atas/telemetry_atas_eth_cmd.hpp"
#include "command_processing/commands_zoo/eth_cmd_factory.hpp"
#include "command_processing/eth_cmd_classification.hpp"
#include <memory>
#include <cstdint>

int main(int argc, char* argv[])
{
    uint8_t* cmd_buffer = new uint8_t[100]; 

    eth_cmd_processing::EthCmdDataTelemetryAtas cmd_tx_data;
    cmd_tx_data.fps_device = 30;
    cmd_tx_data.fps_scan = 2;
    cmd_tx_data.fps_yolo = 150;

    std::shared_ptr<eth_cmd_processing::EthCmdTelemetryAtas> cmd_tx, cmd_rx_ta;
    std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd_rx_base;
    cmd_tx = std::make_shared<eth_cmd_processing::EthCmdTelemetryAtas>();

    cmd_tx->set_data(cmd_tx_data);
    int tx_buf_size;
    cmd_tx->serialize_noalloc(cmd_buffer, tx_buf_size);

    std::cout << "tx_buf_size = " << tx_buf_size << std::endl;

    eth_cmd_processing::EthCmdDemux demux;
    demux.eth_cmd_demux_to_handler(cmd_buffer, cmd_rx_base);
    cmd_rx_ta = std::dynamic_pointer_cast<eth_cmd_processing::EthCmdTelemetryAtas>(cmd_rx_base);
    std::cout << cmd_rx_ta->get_cmd_data().fps_device << std::endl;
return 0;
}
