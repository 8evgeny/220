#ifndef OwletSERVER_H
#define OwletSERVER_H

#include <mutex>

#include "eth/server.h"


class OwletServer : public Server
{
public:
    OwletServer();
    ~OwletServer() override;

private:
    void handle(const SocketAddress & address, const void * data,
                const int & data_length) override;
};

#endif // OwletSERVER_H
