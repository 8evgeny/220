#ifndef CORSAIRSERVER_H
#define CORSAIRSERVER_H

#include <mutex>

class UDPClient;

#ifdef PCAP
#include "socket/pcapserver.h"
class CorsairServer : public PcapServer
#else
#include "socket/udpserver.h"
class CorsairServer : public UDPServer
#endif
{
public:
    CorsairServer();
    ~CorsairServer() override;
    uint8_t * rcv_cmd(int & data_length);
    uint8_t * rcv_frame(int & w, int & h, int & id, int & num);
    int get_frame_num() const {return frame_num;}

    void openMirror(const std::string src, const std::string dst);


private:
    void handle(const SocketAddress & address, const void * data, const int & data_length) override;
private:
    std::string dst_ip = "192.168.0.6";
    std::string src_ip = "192.168.0.5";
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

    UDPClient *m_wslTransmetter{nullptr};
};

#endif // CORSAIRSERVER_H
