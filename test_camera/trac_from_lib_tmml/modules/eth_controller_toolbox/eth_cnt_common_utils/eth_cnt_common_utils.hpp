#ifndef ETH_CNT_COMMON_UTILS_HPP
#define ETH_CNT_COMMON_UTILS_HPP

#include "../eth_cntrl_common_types.hpp"
#include <string>

namespace eth_common::utils
{
    bool convertCommunicationModeStr2enum(const std::string src, eth_common::CommunicationModes &dst);
} // -- END namespace eth_common_utils

#endif //ETH_CNT_COMMON_UTILS_HPP