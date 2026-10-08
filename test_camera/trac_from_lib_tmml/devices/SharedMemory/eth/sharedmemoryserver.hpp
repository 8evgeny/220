#ifndef SHAREDMEMORYSERVER_HPP
#define SHAREDMEMORYSERVER_HPP

#include "eth/server.h"

class SharedMemoryServer : public Server
{
public:
    SharedMemoryServer();
    ~SharedMemoryServer() override;

private:
    void handle(const SocketAddress & address, const void * data,
                const int & data_length) override;

private:
    const int CMD_BUF_LEN = 1024;
};

#endif // SHAREDMEMORYSERVER_HPP
