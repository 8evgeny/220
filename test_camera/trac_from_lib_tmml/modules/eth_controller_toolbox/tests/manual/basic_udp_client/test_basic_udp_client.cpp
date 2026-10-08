#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include "basic_toolbox/cmd_udp_client/cmd_udp_client.hpp"

int main(int argc, char* argv[])
{
    bool ok = false;
    CmdUDPClient cli("127.0.0.1", 25025, ok);
    if(!ok)
    {
        return 1;
    }
    std::cout << "END main" << std::endl;

    std::vector<uint8_t> cmd_tmp(3);
    cmd_tmp[0] = 155;
    cmd_tmp[1] = 99;
    size_t counter = 0;
    while(true)
    {
        cmd_tmp[2] = (counter++) % 255;
        std::cout << (int)cmd_tmp[2] << std::endl;
        cli.snd_cmd(cmd_tmp);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    return 0;
} // -- END main
