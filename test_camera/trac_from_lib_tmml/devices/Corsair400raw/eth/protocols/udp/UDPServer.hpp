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
#include "eth/EthGlobal.hpp"
#include "eth/base/BaseThread.hpp"
#include "eth/base/SocketAddress.hpp"
#include "eth/protocols/udp/UDPSocket.hpp"

class UDPSocket;

class UDPServer: public BaseThread
{
public:
    /**
     *  Constructor
     */
    explicit UDPServer(const uint32_t& rcvBufSize = 10240, const uint32_t& sndBufSize = 1024);
    /**
     *  Desstructor
     */
    virtual ~UDPServer();
    /** Copy and assignment not allowed */
    UDPServer(const UDPServer& ) = delete;
    void operator=(const UDPServer& ) = delete;
    /**
     *  Create socket, bind to srcAddress and start server thread.
     *
     *  @param srcAddress source address (this host)
     *  @param dstAddress destination address (connected device)
     */
    void open(const SocketAddress& srcAddress, const SocketAddress & dstAddress);
    /**
     *  Stop server thread and close socket.
     */
    void close();
    /**
     *  Get connection status.
     *
     *  @return true if socket bind and thread run
     */
    bool isConnected();
    /**
     *  Send data.
     *
     *  @return (byte sent == length) means true
     */
    bool send(const uint8_t* data, const int32_t& length);

    int32_t cntFrame() const {return _cntFrame;}
    int32_t fps()      const {return _fps;}

protected:
    /**
     *  Received data handler. (It's empty. To override)
     *  Called on the thread (UDPServer::runThd()) when data is received.
     *
     *  @param address - socket bind address
     *  @param data    - received data
     *  @param length  - received data length
     */
    virtual void handle(const SocketAddress& address, const uint8_t* data, const uint32_t& length) = 0;
    /**
     *  Compare package header flag and packet size
     *
     *  @param par    - structure with package parameters
     *  @param flag   - package header flag
     *  @param length - packet size
     *  @return true if it matches
     */
    inline bool packParCheck(const UDPPacketParametrs& par, const uint16_t& flag, const uint16_t& length) const{return (flag == par.flag) && (length >= par.dataLength);}

private:
    /**
     *  Initialization server data before thread loop
     *  Include in "BaseThread::run()"
     *
     *  @return true - if successful
     */
    bool initThd() override;
    /**
     *  Server thread loop.
     *  Include in "BaseThread::run()"
     */
    void runThd() override;
    /**
     *  Deinit server data after leaving the thread loop.
     *  Include in "BaseThread::run()"
     */
    void closeThd() override;

protected:
    /** Receive buffer size [byte] */
    uint32_t _rcvBufSize;
    /** Buffer for receiving packets */
    uint8_t* _rcvBuf;
    /** Send buffer size [byte] */
    uint32_t _sndBufSize;
    /** Buffer for sending packets */
    uint8_t* _sndBuf;
    /** Source address (this host) */
    SocketAddress _srcAddress;
    /** Destination address (connected device) */
    SocketAddress _dstAddress;
    /** Socket */
    UDPSocket _socket;
    /** Receive packet length [byte] */
    int32_t   _rcvLength;

    int32_t _cntFrame;
    int32_t _cntFrameLast;
    int32_t _fps;
    std::chrono::steady_clock::time_point _timePoint;

};

#endif // UDPSERVER_H
