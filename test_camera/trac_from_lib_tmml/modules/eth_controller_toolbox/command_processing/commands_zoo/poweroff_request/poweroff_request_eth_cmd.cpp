#include "poweroff_request_eth_cmd.hpp"
#include "../../eth_cmd_common_types.hpp"
#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{
    EthCmdRequestPoweroff::EthCmdRequestPoweroff()
    {
        serialize_size_bytes = 1; // 1 байт - id команды
        cmd_id = ETH_CMD_POWEROFF_REQUEST;
    } // -- END EthCmdRequestPoweroff

    int EthCmdRequestPoweroff::get_serialize_size() const
    {
        return serialize_size_bytes;
    } // -- END get_serialize_size

    bool EthCmdRequestPoweroff::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
    {
        *cmd_buffer = cmd_id;
        buffer_size_bytes = serialize_size_bytes;
        return true;
    } // -- END serialize_noalloc

    bool EthCmdRequestPoweroff::deserialize(uint8_t const * cmd_buffer)
    {
        cmd_buffer++;
        return true;
    } // -- END deserialize
    
} // -- namespace eth_cmd_processing