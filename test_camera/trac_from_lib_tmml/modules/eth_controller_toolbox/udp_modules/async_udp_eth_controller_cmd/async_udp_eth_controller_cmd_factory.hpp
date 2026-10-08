#ifndef ASYNC_UDP_ETH_CONTROLLER_CMD_FACTORY_HPP
#define ASYNC_UDP_ETH_CONTROLLER_CMD_FACTORY_HPP

#include "async_udp_eth_controller_cmd.hpp"

namespace eth_controller::cmd::async::udp
{
    std::shared_ptr<AsyncUDPEthControllerCmd> create(
        const std::string &path2ini, const std::string &ini_section_name);
} // -- namespace eth_controller::cmd::async::udp

#endif // ASYNC_UDP_ETH_CONTROLLER_CMD_FACTORY_HPP