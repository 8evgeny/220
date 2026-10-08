#ifndef CORSAIRSERVER_H
#define CORSAIRSERVER_H

#include <mutex>

#ifdef PCAP
#include "socket/PCAPServer.h"
class CorsairServer : public PcapServer
#else
#include "eth/protocols/udp/UDPServer.hpp"
class CorsairServer : public UDPServer
#endif
{
public:
    CorsairServer();
    ~CorsairServer();
    uint8_t * rcv_cmd(int & data_length);
    uint8_t * rcv_frame(int & w, int & h, int & id, int & num);
    int get_frame_num() const {return frame_num;}

private:
    void handle(const SocketAddress& address, const uint8_t* data, const uint32_t& length) override;

private:
    uint8_t * cmd_buf;
    uint8_t * cmd_rdy;
    int cmd_length;
    bool cmd_ready;
    std::mutex cmd_mutex;
    uint8_t * frame;
    uint8_t * frame_buf;
    uint8_t * frame_rdy;
    int frame_w;
    int frame_h;
    int frame_id;
    int frame_num;
    bool frame_ready;
    std::mutex frame_mutex;

    // UDPServer interface
public:
    void resetData();
};

#endif // CORSAIRSERVER_H
