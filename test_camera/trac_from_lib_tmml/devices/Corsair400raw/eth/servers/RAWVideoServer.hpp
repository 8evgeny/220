#ifndef RAWVIDEOSERVER_H
#define RAWVIDEOSERVER_H

#include "eth/EthGlobal.hpp"
#include "eth/base/TripleBuffer.hpp"
#include "eth/protocols/udp/UDPServer.hpp"
#include <cstdint>

class RAWVideoServer : public UDPServer
{
public:
    /**
     *  Constructor
     */
    explicit RAWVideoServer(const int32_t& serviceMsgFlag    = UDP_SRV_FLAG,
                         const int32_t& serviceMsgLength     = UDP_SRV_LEN,
                         const int32_t& serviceMsgDataLength = UDP_SRV_DATA_LEN,
                         const int32_t& videoMsgFlag         = UDP_VID_FLAG,
                         const int32_t& videoMsgLength       = UDP_VID_LEN,
                         const int32_t& videoMsgDataLength   = UDP_VID_DATA_LEN,
                         const uint32_t& bufferSize          = FRAME_BUF_LEN);
    explicit RAWVideoServer(const UDPPacketParametrs& srvPacketPar,
                            const UDPPacketParametrs& vidPacketPar,
                            const uint32_t& vidBudSize);
    /**
     *  Desstructor
     */
    ~RAWVideoServer();
    /** Copy and assignment not allowed */
    RAWVideoServer(const RAWVideoServer& ) = delete;
    void operator=(const RAWVideoServer& ) = delete;
    /**
     *  Get frame data
     *
     *  @param length - received data length
     *  @return frame data (pointer to data in _dataBuf)
     */
    uint8_t* get(int32_t& length, FrameParametrs& framePar);
    int32_t cntFrameHeader()        const {return _cntFrameHeader;}
    int32_t cntFrameHeaderBroken()  const {return _cntFrameHeaderBroken;}
    int32_t cntFragmentBroken()     const {return _cntFragmentBroken;}

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
    /**
     *  Save frame parametr in _framePar
     *
     *  @param width       - frame width
     *  @param heigh       - frame heigh
     *  @param id          - channel id
     *  @param frameNumber - external frame counter
     */
    void saveFramePar(const int32_t& width, const int32_t& heigh, const int32_t& id, const int32_t& frameNumber);
    /**
     *  Saves the collected frame in _vidBuf
     */
    void saveFrame();

private:
    /** Service packet parameters */
    UDPPacketParametrs _srvMsgPar;
    /** Video packet parameters */
    UDPPacketParametrs _vidMsgPar;
    /** Receive frame buffer */
    TripleBuffer _vidBuf;
    /** Frame parameters from service message */
    FrameParametrs _framePar;

    int32_t _cntFrameHeader;
    int32_t _cntFrameHeaderBroken;
    int32_t _cntFragmentBroken;
};

#endif // RAWVIDEOSERVER_H
