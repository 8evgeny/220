#include "uncomp_frame_eth_cmd.hpp"
#include "../../eth_cmd_common_types.hpp"

namespace eth_cmd_processing
{
    EthCmdUncompFrame::EthCmdUncompFrame()
    {
        cmd_id = ETH_CMD_UNCOMPRESS_FRAME_DEFAULT;
    }
    bool EthCmdUncompFrame::deserialize(uint8_t const *cmd_buffer)
    {
        return false;
    }
} // -- END eth_controller::cmd
