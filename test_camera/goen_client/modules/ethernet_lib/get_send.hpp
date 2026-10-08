#pragma once
#ifndef get_send_H
#define get_send_H

#include <stdlib.h>
#include<iomanip>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <string>
#include <condition_variable>
#include <mutex>
#include <future>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include "INIReader.h"
#include <opencv2/core/utility.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/highgui.hpp"
#include "from_goen_struct.hpp"
#include "to_goen_struct.hpp"
#include "bool_uint_transformer.hpp"



class get_send_data
{
  public:
    get_send_data(bool& ok, const std::string& way2ini);
    ~get_send_data();
    bool eth_open();
    void eth_close();
    std::string parce_IP(const std::string &);
    void set_cmd(ToGoenCommand &cmd);
#ifdef USE_CONFIRMATION
    void get_receive_confirmation(SingleStatusReturn& ssr);
#endif
    void get_receive_telemetry(FromGoenTelemetry& fgt_str);
    void receive_data();

    struct Sync
    {
        std::atomic_bool f_recv_exec = {false};
        std::atomic_bool f_recv_telemetry = {false};
        std::atomic_bool f_recv_bind = {false};
        std::atomic_bool f_send_exec = {false};
        std::atomic_bool f_send_cmd = {false};
#ifdef USE_CONFIRMATION
        std::atomic_bool f_recv_ssr = {false};
#endif  // END ifdef USE_CONFIRMATION
    } sync;  // END struct Sync

  private:
    int goen_send_commands_port = 0;
    int soc_out = -1;
    struct sockaddr_in addr_out;
    int soc_in = -1;
    sockaddr_in addr_in;
    int client_receive_telemetry_port = 0;
    std::string send_ip;

    static const size_t to_goen_size = sizeof(ToGoenCommand);
    uint8_t to_goen_send_buffer[to_goen_size];
    const int to_goen_send_check_sum_idx = to_goen_size - 1; // Индекс элемента буффера, куда записано значение checksum

    static const size_t from_goen_telemetry_size = sizeof(FromGoenTelemetry);    
    uint8_t from_goen_telemetry_buffer[from_goen_telemetry_size];
    const int telemetry_check_sum_idx = from_goen_telemetry_size - 1; // Индекс элемента буффера, куда записано значение checksum
    const size_t telemetry_check_sum_size = from_goen_telemetry_size - 1;  // Размер буффера, передавайемый для расчета xheck_sum

#ifdef USE_CONFIRMATION
    static const size_t ssr_size = sizeof(SingleStatusReturn);
    uint8_t ssr_buffer[ssr_size];
    const int ssr_check_sum_idx = ssr_size - 1; // Индекс элемента буффера, куда записано значение checksum
    const size_t ssr_check_sum_size = ssr_size - 1; // Размер буффера, передавайемый для расчета xheck_sum
    SingleStatusReturn ssr_str;
#endif  // END ifdef USE_CONFIRMATION

    ToGoenCommand to_goen_cmd_str;
    FromGoenTelemetry from_goen_telemetry_str;

    int soc_in_ssr = -1;
    int client_receive_confirmation_port = 0;
    sockaddr_in addr_in_ssr;

    bool get_ini_params(const std::string& config_path);    
    void thr_send_func();
    int send_data();
#ifdef USE_CONFIRMATION
    void receive_confirmation();
#endif
    void receive_telemetry();
    uint8_t check_sum(const uint8_t * buf, const size_t buf_size);

}; // END class get_send_data
#endif // END #ifndef get_send_H
