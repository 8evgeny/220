#ifndef TCPSERVER_H
#define TCPSERVER_H
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

#include "tcpsocket.h"
#include <atomic>
#include <thread>

class TCPServer: public TCPSocket
{
public:
    TCPServer():TCPSocket(),_execute(false){}
    bool opn(const SocketAddress & address) override;
    void cls() override;
protected:
    virtual void handle(const TCPSocket & client, const void * data, const int & data_length);
private:
    void run();
private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

#endif // TCPSERVER_H
