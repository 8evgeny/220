#include "sharedmemoryserver.hpp"

SharedMemoryServer::SharedMemoryServer()
{
    cmd_buf = new uint8_t[CMD_BUF_LEN];
    cmd_rdy = new uint8_t[CMD_BUF_LEN];
    cmd_length = 0;
    cmd_ready = false;
}

SharedMemoryServer::~SharedMemoryServer()
{
    cls();
    cmd_mutex.lock();
    cmd_mutex.unlock();
    delete [] cmd_buf;
    delete [] cmd_rdy;
}

void SharedMemoryServer::handle(const SocketAddress &address,
                                const void *data, const int &data_length)
{
    const uint8_t * cmd = reinterpret_cast<const uint8_t *>(data);
    for(auto handler : cmd_handlers)
    {
        handler->handle(std::vector<uint8_t>(cmd, cmd + data_length));
    }
}
