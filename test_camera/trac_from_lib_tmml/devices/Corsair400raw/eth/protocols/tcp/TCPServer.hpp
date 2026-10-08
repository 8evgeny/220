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
#include "eth/base/BaseThread.hpp"
#include "eth/base/SocketAddress.hpp"

class TCPServer: public BaseThread
{
public:
    explicit TCPServer();
    virtual ~TCPServer();
    // copy and assignment not allowed
    TCPServer(const TCPServer& ) = delete;
    void operator=(const TCPServer& ) = delete;

    bool open(const SocketAddress& srcAddress, const SocketAddress & dstAddress);
    void close() ;
    bool isConnected();
    uint8_t* get(int32_t& length);

    bool send(const uint8_t* data, const int32_t& length);

protected:
    virtual void handle(const SocketAddress&, const void * data, const int32_t& length);

private:
    void run();

};

#endif // TCPSERVER_H
