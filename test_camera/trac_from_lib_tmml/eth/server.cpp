#include "server.h"


Server::~Server()
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

uint8_t *Server::rcv_cmd(int &data_length)
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

uint8_t *Server::rcv_frame(int &w, int &h, int &id, int &num)
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

std::vector<ICommandHandler *> *Server::getHandlers()
{
    return &cmd_handlers;
}

void Server::register_frame_handler(IFrameHandler *handler)
{
    frame_handlers.push_back(handler);
}

void Server::register_command_handler(ICommandHandler *handler)
{
    cmd_handlers.push_back(handler);
}
