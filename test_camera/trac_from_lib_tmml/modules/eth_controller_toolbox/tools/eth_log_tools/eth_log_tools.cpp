#include "eth_log_tools.hpp"

std::string sockinfo_to_str(const std::string &ip, int port)
{
    std::string ret = "[ip=" + ip + "; port=" + std::to_string(port) + "]";
    return ret;
} // -- END sockinfo_to_str