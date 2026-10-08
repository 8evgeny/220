#ifndef CORSAIR400RAW_SETTINGS_HPP
#define CORSAIR400RAW_SETTINGS_HPP

#include <string>

namespace devices::corsair_400_raw
{
    struct NetSettings
    {
        std::string     rcv_ip      = "192.168.1.35";
        int             rcv_port    = 52515;
    };

    struct Settings
    {
        std::string                 path2ini;
        std::string                 section_name;
        NetSettings                 net;
    };

} // -- namespace devices::corsair_400_raw

#endif // CORSAIR400RAW_SETTINGS_HPP
