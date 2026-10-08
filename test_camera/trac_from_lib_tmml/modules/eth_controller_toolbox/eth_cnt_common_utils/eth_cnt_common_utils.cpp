#include "eth_cnt_common_utils.hpp"

namespace eth_common::utils
{
    bool convertCommunicationModeStr2enum(
        const std::string src, eth_common::CommunicationModes &dst)
    {
        if(src == "simplex_transmit")
        {
            dst = eth_common::CommunicationModes::SIMPLEX_TRANSMIT;
        }
        else if(src == "simplex_receive")
        {
            dst= eth_common::CommunicationModes::SIMPLEX_RECEIVE;
        }
        else if(src == "duplex")
        {
            dst = eth_common::CommunicationModes::DUPLEX;
        }
        else
        {
            dst = eth_common::CommunicationModes::NOT_DEFINE;
            return false;
        }
        return true;
    } // -- END convertCommunicationModeStr2enum
} // namespace eth_common::utils