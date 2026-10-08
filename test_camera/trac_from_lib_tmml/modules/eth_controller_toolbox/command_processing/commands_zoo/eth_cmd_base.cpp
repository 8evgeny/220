#include "eth_cmd_base.hpp"
#include "../eth_cmd_common_types.hpp"

namespace eth_cmd_processing
{
    EthCmdBase::EthCmdBase()
    {
        cmd_id = ETH_CMD_NO_COMMAND; // на это значение будет проверка при перераспределении команды по handler'ам
    }

    EthCommands EthCmdBase::get_id() const
    {
        return cmd_id;
    }

    int EthCmdBase::get_serialize_size() const
    {
        return 0;
    }

    bool EthCmdBase::serialize_alloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
    {
        return false;
    }

    bool EthCmdBase::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
    {
        return false;
    }

    bool EthCmdBase::deserialize(uint8_t const *cmd_buffer)
    {
        return false;
    }

}