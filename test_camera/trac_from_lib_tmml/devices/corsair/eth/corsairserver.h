#ifndef CORSAIRSERVER_H
#define CORSAIRSERVER_H

#include <mutex>

#include "eth/server.h"


class CorsairServer : public Server
{
public:
    CorsairServer();
    ~CorsairServer() override;

private:
    void handle(const SocketAddress & address, const void * data,
                const int & data_length) override;
};

#endif // CORSAIRSERVER_H
