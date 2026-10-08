#include "corsairserver.hpp"

#include "eth/EthGlobal.hpp"
#include <string.h>>

#define CMD_BUF_LEN   (1024)
#define FRAME_BUF_W   (1920)
#define FRAME_BUF_H   (1080)
#define FRAME_BUF_LEN (FRAME_BUF_W * FRAME_BUF_H)

CorsairServer::CorsairServer():
#ifdef PCAP
    PcapServer(),
#else
    UDPServer(),
#endif
    cmd_buf(new uint8_t[CMD_BUF_LEN]),
    cmd_rdy(new uint8_t[CMD_BUF_LEN]),
    cmd_length(0),
    cmd_ready(false),
    frame(new uint8_t[FRAME_BUF_LEN]),
    frame_buf(new uint8_t[FRAME_BUF_LEN]),
    frame_rdy(new uint8_t[FRAME_BUF_LEN]),
    frame_w(FRAME_BUF_W),
    frame_h(FRAME_BUF_H),
    frame_id(0),
    frame_num(0),
    frame_ready(false)
{

}

CorsairServer::~CorsairServer()
{
    UDPServer::close();
    cmd_mutex.lock();
    cmd_mutex.unlock();
    delete [] cmd_buf;
    delete [] cmd_rdy;
    frame_mutex.lock();
    frame_mutex.unlock();
    delete [] frame;
    delete [] frame_buf;
    delete [] frame_rdy;
}
uint8_t * CorsairServer::rcv_cmd(int & data_length)
{
    cmd_mutex.lock();
    if(!cmd_ready)
    {
        cmd_mutex.unlock();
        return nullptr;
    }
    data_length = cmd_length;
    uint8_t * tmp = cmd_rdy;
    cmd_rdy = cmd_buf;
    cmd_buf = tmp;
    cmd_ready = false;
    cmd_mutex.unlock();
    return cmd_rdy;
}
uint8_t * CorsairServer::rcv_frame(int & w, int & h, int & id, int & num)
{
    frame_mutex.lock();
    if(!frame_ready)
    {
        frame_mutex.unlock();
        return nullptr;
    }
    w = frame_w;
    h = frame_h;
    id = frame_id;
    num = frame_num;
    uint8_t * tmp = frame_rdy;
    frame_rdy = frame_buf;
    frame_buf = tmp;
    frame_ready = false;
    frame_mutex.unlock();
    return frame_rdy;
}
void CorsairServer::handle(const SocketAddress&, const uint8_t* data, const uint32_t& length)
{
    static int num = 0;
    static int w = 0;
    static int h = 0;
    static int id = 0;
    static int fragment = 0;
    static int fragments = 0;
    static bool frame_flag = false;
    const srv_packet * packet = reinterpret_cast<const srv_packet *>(data);
    if( (packet->flag == UDP_CMD_FLAG) && (length >= UDP_CMD_LEN) )  // Command packet
    {
        const cmd_packet * packet = reinterpret_cast<const cmd_packet *>(data);
        cmd_mutex.lock();
        cmd_length = UDP_CMD_DATA_LEN;
        memcpy(cmd_buf, packet->data, UDP_CMD_DATA_LEN);
        cmd_ready = true;
        cmd_mutex.unlock();
    }
    else if( (packet->flag == UDP_SRV_FLAG) && (length >= UDP_SRV_LEN) )  // Service packet
    {
        num = packet->frameNumber;
        w = packet->width;
        h = packet->heigh;
        id = packet->id;
        fragments = w * h / UDP_VID_DATA_LEN;

        frame_flag = true;
    }
    else if( frame_flag && (packet->flag == UDP_VID_FLAG) && (length >= UDP_VID_LEN) )  // Video packet
    {
        const vid_packet * packet = reinterpret_cast<const vid_packet *>(data);
        // Check frame is continous (next_frame_fragment = prev_frame_fragment + 1)
        int next_fragment = packet->fragment_number;
        if((next_fragment > 0) && ((fragment + 1) != next_fragment))
        {
            frame_flag = false;
            return;
        }
        fragment = next_fragment;
        // Check frame is not out of range
        if(fragment >= fragments)
        {
            frame_flag = false;
            return;
        }
        // Copy received fragment to frame container
        const int offset = fragment * UDP_VID_DATA_LEN;
        memcpy(frame + offset, packet->data, UDP_VID_DATA_LEN);
        // If received frame is ready then send it to process
        if(fragment == (fragments - 1))
        {
            frame_mutex.lock();
            frame_num = num;
            frame_w = w;
            frame_h = h;
            frame_id = id;
            uint8_t * tmp = frame_buf;
            frame_buf = frame;
            frame = tmp;
            frame_ready = true;
            frame_mutex.unlock();
            frame_flag = false;
        }
    }
}

void CorsairServer::resetData()
{

}
