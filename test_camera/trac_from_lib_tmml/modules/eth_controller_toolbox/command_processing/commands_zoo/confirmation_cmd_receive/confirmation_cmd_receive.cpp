#include "confirmation_cmd_receive.hpp"
#include <cstring>
#include "../../eth_serialization_tools.hpp"

// id_cmd(uint8_t), receive_cmd_id(uint8_t), token_size(int32_t), cmd_buffer(token_size * uint8_t)

namespace eth_cmd_processing
{
    EthCmdConfirmCmdReceive::EthCmdConfirmCmdReceive()
    {
        cmd_id = ETH_CMD_CONFIRM_CMD_RECEIVE;
    }

    EthCmdConfirmCmdReceive::~EthCmdConfirmCmdReceive()
    {

    }

    int EthCmdConfirmCmdReceive::get_serialize_size() const
    {
        return serialize_size_bytes;
    }

    bool EthCmdConfirmCmdReceive::serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const
    {
        *cmd_buffer = static_cast<uint8_t>(cmd_id); cmd_buffer++;
        int s0 = 1;
        *cmd_buffer = static_cast<uint8_t>(receive_cmd_id); cmd_buffer++;
        int s1 = 1; 
        int s2 = eth_serialization::little_endian::conv2byteArrPushBack_int32t(token.size(), cmd_buffer);
        uint8_t const * token_ptr = token.data();
        std::memcpy(cmd_buffer, token_ptr, token.size()); cmd_buffer += token.size();
        buffer_size_bytes = s0 + s1 + s2 + token.size();
        return true;
    }

    bool EthCmdConfirmCmdReceive::deserialize(uint8_t const *cmd_buffer)
    {
        cmd_buffer++; // skip cmd_id
        receive_cmd_id = static_cast<EthCommands>(*cmd_buffer); cmd_buffer++;
        int token_size_tmp = eth_deserialization::little_endian::parse_int32(cmd_buffer);
        token = std::vector<uint8_t>(cmd_buffer, cmd_buffer + token_size_tmp);
        serialize_size_bytes = 1 + 1 + 4 + token_size_tmp;
        return true;
    }

    bool EthCmdConfirmCmdReceive::set_data(EthCommands _receive_cmd_id, const std::vector<uint8_t> &_token)
    {
        receive_cmd_id = _receive_cmd_id;
        this->token = _token;
        return true;
    }

    void EthCmdConfirmCmdReceive::get_data(EthCommands &_receive_cmd_id, std::vector<uint8_t> &_token) const
    {
        _receive_cmd_id = receive_cmd_id;
        _token = token;
    }
}