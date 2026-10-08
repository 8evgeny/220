#ifndef RTP_ETH_CONTROLLER_FACTORY_HPP
#define RTP_ETH_CONTROLLER_FACTORY_HPP

#include "gstcv_mjpg_eth_controller.hpp"

namespace rtp_eth::controller_V1
{
    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &ini_section_name, 
        RTPEthCntSettings &settings);
    bool checkValidIniSettings(RTPEthCntSettings &settings);
    std::shared_ptr<MJPGEthController> create(
        const std::string &path2ini, 
        const std::string &ini_section_name);
} // namespace rtp_eth::controller_V1


#endif //RTP_ETH_CONTROLLER_FACTORY_HPP
