#ifndef PCAPSERVER_H
#define PCAPSERVER_H
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

#include "pcapsocket.h"
#include <atomic>
#include <thread>

class PcapServer: public PcapSocket
{
public:
    PcapServer():PcapSocket(),_execute(false){}
    bool opn(const SocketAddress & address) override;
    void cls() override;
private:
    void run();
    virtual void handle(const SocketAddress & address, const void * data, const int & data_length);
private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

#endif // PCAPSERVER_H
