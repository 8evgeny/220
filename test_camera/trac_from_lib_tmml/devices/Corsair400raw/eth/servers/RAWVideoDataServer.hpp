#ifndef RAWVIDEODATASERVER_HPP
#define RAWVIDEODATASERVER_HPP

#include "eth/EthGlobal.hpp"
#include "eth/base/TripleBuffer.hpp"
#include "eth/protocols/udp/UDPServer.hpp"
#include <cstdint>

class RAWVideoDataServer : public UDPServer
{
public:
    /**
     *  Constructor
     *
     *  @param msgPacketPar - data packet parametrs
     *  @param srvPacketPar - service packet parametrs
     *  @param vidPacketPar - video packet parametrs
     *  @param dataBufSize  - data buffer size [byte]
     *  @param vidBudSize   - frame buffer size [byte]
     */
    explicit RAWVideoDataServer(const UDPPacketParametrs& msgPacketPar,
                                const UDPPacketParametrs& srvPacketPar,
                                const UDPPacketParametrs& vidPacketPar,
                                const uint32_t& dataBufSize,
                                const uint32_t& vidBudSize);
    /**
     *  Desstructor
     */
    ~RAWVideoDataServer();
    /** Copy and assignment not allowed */
    RAWVideoDataServer(const RAWVideoDataServer& ) = delete;
    void operator=(const RAWVideoDataServer& ) = delete;
    /**
     *  Get msg data
     *
     *  @param length - received data length
     *  @return msg data (pointer to data in _dataBuf)
     */
    uint8_t* getMsg(int32_t& length);
    /**
     *  Get frame data
     *
     *  @param length   - received data length
     *  @param framePar - current frame parametrs (width, ..)
     *  @return frame data (pointer to data in _vidBuf)
     */
    uint8_t* getFrame(int32_t& length, FrameParametrs& framePar);
    /**
     *  Send data with packet header.
     *
     *  @param data    - sending data
     *  @param length  - sending data length [byte]
     *  @return (byte sent == length) means true
     */
    bool sendCmd(const uint8_t* data, const int32_t& length);

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
    /**
     *  Save frame parametr in _framePar
     *
     *  @param width       - frame width
     *  @param heigh       - frame heigh
     *  @param id          - channel id
     *  @param frameNumber - external frame counter
     */
    void saveFramePar(const int32_t& width, const int32_t& heigh, const int32_t& id, const uint32_t& frameNumber);
    /**
     *  Saves the collected frame in _vidBuf
     */
    void saveFrame();

private:
    /** Msg packet parameters */
    UDPPacketParametrs _msgPar;
    /** Service packet parameters */
    UDPPacketParametrs _srvMsgPar;
    /** Video packet parameters */
    UDPPacketParametrs _vidMsgPar;
    /** Msg receiving buffer */
    TripleBuffer _dataBuf;
    /** Receive frame buffer */
    TripleBuffer _vidBuf;
    /** Frame parameters from service message */
    FrameParametrs _framePar;
};

#endif // RAWVIDEODATASERVER_HPP
