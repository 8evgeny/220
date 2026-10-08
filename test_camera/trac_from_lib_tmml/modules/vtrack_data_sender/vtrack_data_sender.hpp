#ifndef VTRACK_DATA_SENDER_HPP
#define VTRACK_DATA_SENDER_HPP

#include "vtrack_data_struct.hpp"

#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif // __linux__

namespace track_pos_sender
{

class vtrackDataSender
{
public:
    vtrackDataSender(const Settings & settings_, bool & ok);
    ~vtrackDataSender();
    int send(const vtrackDataStruct & data);
    int send(const StateCmdStruct & data);
    void log_send_cmd(); // логирование отправляемых UDP пакетов в целях отладки
private:
    bool serialize(const vtrackDataStruct & data);
    bool serialize(const StateCmdStruct & data);
    // bool  deserialize(vtrackDataStruct & data);
    Settings settings;
    int sockfd;
    struct sockaddr_in servaddr;

    uint8_t * cmd_buffer = nullptr; // байтовый буфер команды
    int sz_buf_cur = -1; // текущий размер байтового буфера, зависит от количества элементов вектора
    int sz_buf_max = -1; // максимальный размер байтового буфера, вычисляется на основании максимально возможной длины вектора (max_size_vec)
    int max_size_vec = 256; // uint8_t limited 
}; // END class vtrackDataSender

}; // END track_pos_sender

#endif // VTRACK_DATA_SENDER_HPP
