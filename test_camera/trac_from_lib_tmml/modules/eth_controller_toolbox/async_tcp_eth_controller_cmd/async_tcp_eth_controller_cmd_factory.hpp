#ifndef ASYNC_TCP_ETH_CNT_CMD_FACTORY_HPP
#define ASYNC_TCP_ETH_CNT_CMD_FACTORY_HPP

#include "async_tcp_eth_controller_cmd.hpp"

namespace eth_controller::cmd::async
{
    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        AsEthCntSettingsCmd &settings);
    bool checkValidIniSettings(const AsEthCntSettingsCmd &settings);
    std::shared_ptr<AsyncTcpEthControllerCmd> create(const std::string &path2ini, const std::string &sectionName);
} // -- namespace eth_controller::cmd::async

#endif //ASYNC_TCP_ETH_CNT_CMD_FACTORY_HPP