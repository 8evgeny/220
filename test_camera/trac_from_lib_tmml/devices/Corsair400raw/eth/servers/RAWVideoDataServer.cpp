#include "RAWVideoDataServer.hpp"

#include <string.h>


RAWVideoDataServer::RAWVideoDataServer(const UDPPacketParametrs& msgPacketPar,
                                       const UDPPacketParametrs& srvPacketPar,
                                       const UDPPacketParametrs& vidPacketPar,
                                       const uint32_t& dataBufSize,
                                       const uint32_t& vidBudSize)
    : UDPServer()
    , _msgPar(msgPacketPar)
    , _srvMsgPar(srvPacketPar)
    , _vidMsgPar(vidPacketPar)
    , _dataBuf(dataBufSize)
    , _vidBuf(vidBudSize)
    , _framePar{}
{
#ifdef DEBUG
    if(dataBufSize < _msgPar.dataLength) {
        ERROR_MSG("RAWVideoDataServer: Data buffer size < _msgPar.dataLength");
    }
    if(vidBudSize < _vidMsgPar.dataLength) {
        ERROR_MSG("RAWVideoDataServer: video buffer size < _vidMsgPar.dataLength");
    }
    if((_msgPar.length < 6) || (_msgPar.dataLength < 10) || (_msgPar.length <= _msgPar.dataLength)) {
        ERROR_MSG("RAWVideoDataServer: msg packet size corrupted ");
    }
    if((_srvMsgPar.length < 4) || (_srvMsgPar.dataLength < 8) || (_srvMsgPar.length <= _srvMsgPar.dataLength)) {
        ERROR_MSG("RAWVideoDataServer: srv packet size corrupted ");
    }
    if((_vidMsgPar.length < 28) || (_vidMsgPar.dataLength < 32) || (_vidMsgPar.length <= _vidMsgPar.dataLength)) {
        ERROR_MSG("RAWVideoDataServer: video packet size corrupted ");
    }
#endif
}

RAWVideoDataServer::~RAWVideoDataServer()
{
    UDPServer::close();
}

uint8_t* RAWVideoDataServer::getMsg(int32_t& length)
{
    return _dataBuf.get(length);
}

uint8_t* RAWVideoDataServer::getFrame(int32_t& length, FrameParametrs& framePar)
{
    uint8_t* result = _vidBuf.get(length);
    if(result) {
        framePar.width  = _framePar.width;
        framePar.height  = _framePar.height;
        framePar.id     = _framePar.id;
        framePar.number = _framePar.number;
    } else {
        framePar.width  = 0;
        framePar.height  = 0;
        framePar.id     = 0;
        framePar.number = 0;
    }
    return result;
}

bool RAWVideoDataServer::sendCmd(const uint8_t* data, const int32_t& length)
{
    cmd_packet* packet = reinterpret_cast<cmd_packet*>(_sndBuf);
    packet->preambule = 0;
    packet->flag = _msgPar.flag;
    std::copy(data, data + length, packet->data);
    return send(_sndBuf, sizeof(cmd_packet) + length);
}

void RAWVideoDataServer::handle(const SocketAddress&, const uint8_t* data, const uint32_t& length)
{
    const srv_packet* packet = reinterpret_cast<const srv_packet*>(data);
    if( packParCheck(_msgPar, packet->flag, length)  )  // Command packet
    {
        const cmd_packet * packet = reinterpret_cast<const cmd_packet*>(data);
        _dataBuf.writeData(packet->data, length - _msgPar.headerLength);
        _dataBuf.setReady();
    }
    else if( packParCheck(_srvMsgPar, packet->flag, length) )  // Service packet
    {
        saveFramePar(packet->width, packet->heigh, packet->id, packet->frameNumber);
        _framePar.frameStart = (_framePar.width > 0) && (_framePar.height > 0);
    }
    else if( _framePar.frameStart && packParCheck(_vidMsgPar, packet->flag, length) )  // Video packet
    {
        const vid_packet* packet = reinterpret_cast<const vid_packet *>(data);
        // Check frame is continous (next_frame_fragment = prev_frame_fragment + 1)
        int next_fragment = packet->fragment_number;
        if((next_fragment > 0) && ((_framePar.currFragment + 1) != next_fragment))
        {
            _framePar.frameStart = false;
            ERROR_MSG("RAWVideoDataServer: frame is not continous");
            return;
        }
        _framePar.currFragment = next_fragment;
        // Check frame is not out of range
        if(_framePar.currFragment >= _framePar.fragments)
        {
            _framePar.frameStart = false;
            ERROR_MSG("RAWVideoDataServer: Frame is not out of range");
            return;
        }
        // Copy received fragment to frame container
        _vidBuf.writeData(packet->data, _vidMsgPar.dataLength);
        // If received frame is ready then send it to process
        if(_framePar.currFragment == (_framePar.fragments - 1))
        {
            saveFrame();
        }
    }
}

void RAWVideoDataServer::saveFramePar(const int32_t& width, const int32_t& heigh, const int32_t& id, const uint32_t& frameNumber)
{
    _framePar.number = frameNumber;
    _framePar.width  = width;
    _framePar.height  = heigh;
    _framePar.id     = id;
    _framePar.fragments = (width * heigh) / _vidMsgPar.dataLength;
}

void RAWVideoDataServer::saveFrame()
{
    _vidBuf.setReady();
    _framePar.currFragment = 0;
}
