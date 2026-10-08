#ifndef UDPSERVER_H
#define UDPSERVER_H
/*
 * Example:
 * ...
 * class ConcreteUDPServer: public UDPServer
 * {
 * ...
 * private:
 *     void handle(const SocketAddress & src, const void * data, const int & data_length) override {...}
 * ...
 * };
 * ...
 *
 * */

#include "udpsocket.h"
#include <atomic>
#include <thread>

class UDPServer: public UDPSocket
{
public:
    UDPServer():UDPSocket(),_execute(false){}
    bool opn(const SocketAddress & address) override;
    void cls() override;
protected:
    virtual void handle(const SocketAddress & address, const void * data, const int & data_length);
private:
    void run();
private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

#endif // UDPSERVER_H
