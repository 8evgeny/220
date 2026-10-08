#ifndef DATASERVER_HPP
#define DATASERVER_HPP

#include "eth/EthGlobal.hpp"
#include "eth/base/TripleBuffer.hpp"
#include "eth/protocols/udp/UDPServer.hpp"
#include <cstdint>


class DataServer : public UDPServer
{
public:
    /**
     *  Constructor
     */
    explicit DataServer(const UDPPacketParametrs& msgPacketPar,
                        const uint32_t& dataBufSize);
    /**
     *  Desstructor
     */
    ~DataServer();
    /**
     *  Send data with packet header.
     *
     *  @param data    - sending data
     *  @param length  - sending data length [byte]
     *  @return (byte sent == length) means true
     */
    bool sendCmd(const uint8_t* data, const int32_t& length);
    /**
     *  Get msg data
     *
     *  @param length - received data length
     *  @return msg data (pointer to data in _dataBuf)
     */
    uint8_t* getMsg(int32_t& length);

protected:
    /**
     *  Received data handler.
     *  Called on the thread (UDPServer::runThd()) when data is received.
     *
     *  @param address - socket bind address
     *  @param data    - received data
     *  @param length  - received data length
     */
    void handle(const SocketAddress& address, const uint8_t* data, const uint32_t& length) override;

private:
    /** Msg packet parameters */
    UDPPacketParametrs _msgPar;
    /** Receive data buffer */
    TripleBuffer _dataBuf;
};

#endif // DATASERVER_HPP
