#include "eth_cmd_classification.hpp"

// specific cmd's headers
#include "commands_zoo/copter_alarm/copter_alarm_eth_cmd.hpp"
#include "commands_zoo/uncompress_frame/uncomp_frame_eth_cmd.hpp"


namespace eth_cmd_processing
{
    EthCmdDemux::EthCmdDemux()
    {
        eth_cmd_factory = EthCmdFactory();
    } // -- END EthCmdDemux::EthCmdDemux
    
    bool EthCmdDemux::eth_cmd_demux_to_handler(uint8_t *cmd_buffer, std::shared_ptr<EthCmdBase> &command) const
    {
        uint8_t cmd_id = *cmd_buffer; // парсинг первого байта - 0..255 - id команды
        
        command = eth_cmd_factory.create(static_cast<EthCommands>(cmd_id));
        if (command == nullptr)
        {
            return false;
        }
        command->deserialize(cmd_buffer);

        return true;
    } // -- END EthCmdDemux::eth_cmd_demux_to_handler
}
