#include "owletserver.h"
#include "owlet_global.h"

#include "string.h"

#define OWLET_CMD_BUF_LEN     (1024)
#define OWLET_FRAME_BUF_W     (1920)
#define OWLET_FRAME_BUF_H     (1080)
#define OWLET_FRAME_BUF_LEN   (OWLET_FRAME_BUF_W * OWLET_FRAME_BUF_H)

OwletServer::OwletServer()
{
    cmd_buf = new uint8_t[OWLET_CMD_BUF_LEN];
    cmd_rdy = new uint8_t[OWLET_CMD_BUF_LEN];
    cmd_length = 0;
    cmd_ready = false;
    frame = new uint8_t[OWLET_FRAME_BUF_LEN];
    frame_buf = new uint8_t[OWLET_FRAME_BUF_LEN];
    frame_rdy = new uint8_t[OWLET_FRAME_BUF_LEN];
    frame_w = OWLET_FRAME_BUF_W;
    frame_h = OWLET_FRAME_BUF_H;
    frame_id = 0;
    frame_num = 0;
    frame_ready = false;
}

OwletServer::~OwletServer()
{
    cls();
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

void OwletServer::handle(const SocketAddress &, const void * data, const int & data_length)
{
    static int num = 0;
    static int w = 0;
    static int h = 0;
    static int id = 0;
    static int fragment = 0;
    static int fragments = 0;
    static bool frame_flag = false;
    const srv_packet * packet = reinterpret_cast<const srv_packet *>(data);
//    printf("Packet received, flag %d \n", packet->flag);
    if( (packet->flag == OWLET_UDP_CMD_FLAG) && (data_length >= OWLET_UDP_CMD_LEN) )  // Command packet
    {
        const cmd_packet * packet = reinterpret_cast<const cmd_packet *>(data);
        cmd_mutex.lock();
        cmd_length = OWLET_UDP_CMD_DATA_LEN;
        memcpy(cmd_buf, packet->data, OWLET_UDP_CMD_DATA_LEN);
        cmd_ready = true;
        cmd_mutex.unlock();
//        printf("Command packet received\n");
    }
    else if( (packet->flag == OWLET_UDP_SRV_FLAG) && (data_length >= OWLET_UDP_SRV_LEN) )  // Service packet
    {
        num = packet->frame_number;
        w = packet->w;
        h = packet->h;
        id = packet->id;
        fragments = w * h / OWLET_UDP_VID_DATA_LEN;

        frame_flag = true;
//        printf("Service packet received\n");
    }
    else if( frame_flag && (packet->flag == OWLET_UDP_VID_FLAG) && (data_length >= OWLET_UDP_VID_LEN) )  // Video packet
    {
//        printf("Video packet received\n");
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
        const int offset = fragment * OWLET_UDP_VID_DATA_LEN;
        memcpy(frame + offset, packet->data, OWLET_UDP_VID_DATA_LEN);
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
