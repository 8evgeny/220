#pragma once
#ifndef CMD_OVER_UDP_RECEIVER_HPP
#define CMD_OVER_UDP_RECEIVER_HPP

#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "thread"
#include "unistd.h"
#include <vector>
#include "common_data.hpp"
#include <memory>
#include "to_bort_struct.hpp"
#include "string"
#include <fstream>
#include "INIReader.h"


using namespace std;
class CmdOverUdpReceiver
{
public:
    CmdOverUdpReceiver(std::string& config_path);
    ~CmdOverUdpReceiver();
    void work();
    void start();
    bool receivedCMD = false;
    static constexpr int buf_size = 512;
    unsigned char  buf[buf_size];

    //int send_telemetry(uint8_t * buff_telem);
    int send_telemetry(ToBortTelemetry& to_bort_telemetry_str);
#ifdef USE_CONFIRMATION
    void send_single_status_return_udp(uint8_t cmd_byte);
#endif
#ifdef USE_UDP_TLM
    int send_extension_tlm(Tlm4AVAX& tlm4avax_str_);
#endif // USE_UDP_TLM

private:
    int sockfd = -1;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    int goen_in_port = 0;

#ifdef USE_UDP_TLM
    // Параметры для отправки доп телеметрии на АП
    sockaddr_in addr_out_ap;
    int sock_out_ap = -1;
    int ap_in_port = 0;
    std::string ap_IP = "";
    Tlm4AVAX tlm4avax_str;
    static const size_t size_telemetry_ap = sizeof(Tlm4AVAX);
    uint8_t buff_telemetry_ap[size_telemetry_ap];
#endif // USE_UDP_TLM

    sockaddr_in addr_out;
    static const size_t size_telemetry = sizeof(ToBortTelemetry);
    const int telemetry_check_sum_idx = (int)size_telemetry - 1;
    uint8_t buff_telemetry[size_telemetry];
    int client_in_port = 0;
    std::string send_IP = "";
    int sock_out = -1;
    //int client_in_port = 0;

 #ifdef USE_CONFIRMATION
     static const size_t ssr_buffer_size = sizeof(ReplayToCmdToBort);
     uint8_t ssr_buff[ssr_buffer_size];
 #endif
    bool get_ini_params(const std::string& config_path);
    uint8_t check_sum(const unsigned char *buf, const size_t buf_size);

}; // END class CmdOverUdpReceiver

#endif // CMD_OVER_UDP_RECEIVER_HPP
