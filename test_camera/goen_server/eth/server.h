#ifndef SERVER_H
#define SERVER_H

#include <mutex>
#include "ethcontroller.h"
#include "cmd/icommandhandler.h"
#include "devices/iframehandler.h"

#ifdef PCAP
#include "socket/pcapserver.h"
class Server : public PcapServer
#else
#include "socket/udpserver.h"
class Server : public UDPServer
#endif
{
public:
    Server(){}
    ~Server();
    virtual uint8_t * rcv_cmd(int & data_length);
    virtual uint8_t * rcv_frame(int & w, int & h, int & id, int & num);
    virtual int get_frame_num() const {return frame_num;}
    std::vector<ICommandHandler *> *getHandlers();
    void register_frame_handler(IFrameHandler * handler);
    void register_command_handler(ICommandHandler * handler);
    virtual void quit(){}

protected:
    uint8_t * cmd_buf = nullptr;
    uint8_t * cmd_rdy = nullptr;
    int cmd_length = 0;
    bool cmd_ready = false;
    std::mutex cmd_mutex;
    uint8_t * frame = nullptr;
    uint8_t * frame_buf = nullptr;
    uint8_t * frame_rdy = nullptr;
    int frame_w = 0;
    int frame_h = 0;
    int frame_id = 0;
    int frame_num = 0;
    std::atomic<bool> frame_ready;
    std::mutex frame_mutex;

    std::vector<ICommandHandler *> cmd_handlers;
    std::vector<IFrameHandler *> frame_handlers;
};

#endif // SERVER_H
