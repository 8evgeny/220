#ifndef UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER_FACTORY_HPP
#define UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER_FACTORY_HPP

#include "udp_eth_gstcv_raw_video_receiver.hpp"

namespace eth_video::udp::raw::gstcv::receiver
{
    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        Settings &settings);
    bool checkValidSettings(const Settings &settings);
    std::shared_ptr<UDPEthGstCVRawVideoReceiver> create(
        const std::string &path2ini,
        const std::string &ini_section_name);
} // namespace eth_video::udp::raw::gstcv::receiver


#endif //UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER_FACTORY_HPP