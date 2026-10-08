#include "RAWVideoServer.hpp"
#include <cassert>

RAWVideoServer::RAWVideoServer(const UDPPacketParametrs& srvPacketPar,
                               const UDPPacketParametrs& vidPacketPar,
                               const uint32_t& vidBudSize)
    : UDPServer()
    , _srvMsgPar(srvPacketPar)
    , _vidMsgPar(vidPacketPar)
    , _vidBuf(vidBudSize)
    , _framePar{}
    , _cntFrameHeader{0}
    , _cntFrameHeaderBroken{0}
    , _cntFragmentBroken{0}
{
#ifdef DEBUG
    if(vidBudSize < _vidMsgPar.dataLength) {
        ERROR_MSG("VideoServer: video buffer size < _vidMsgPar.dataLength");
    }
    if((_srvMsgPar.length < 6) || (_srvMsgPar.length < (_srvMsgPar.dataLength + _srvMsgPar.headerLength))) {
        ERROR_MSG("VideoServer: msg packet size corrupted");
    }
    if((_vidMsgPar.length < 28) || (_vidMsgPar.length < (_vidMsgPar.dataLength + _vidMsgPar.headerLength))) {
        ERROR_MSG("VideoServer: video packet size corrupted");
    }
#endif
} // END RAWVideoServer

RAWVideoServer::~RAWVideoServer()
{
    UDPServer::close();
} // END ~RAWVideoServer

uint8_t* RAWVideoServer::get(int32_t& length, FrameParametrs& framePar)
{
    uint8_t* result = _vidBuf.get(length);
    if(result)
    {
        framePar.width  = _framePar.width;
        framePar.height = _framePar.height;
        framePar.id     = _framePar.id;
        framePar.number = _framePar.number;
    } // END  if(result)
    else
    {
        framePar.width  = 0;
        framePar.height = 0;
        framePar.id     = 0;
        framePar.number = 0;
    } // END else
    return result;
} // END get

void RAWVideoServer::handle(const SocketAddress& address, const uint8_t* data, const uint32_t& length)
{
    const srv_packet* packet = reinterpret_cast<const srv_packet*>(data);
    if( packParCheck(_srvMsgPar, packet->flag, length) )  // Service packet
    {
        saveFramePar(packet->width, packet->heigh, packet->id, packet->frameNumber);
         // Check frame size (size >= 16x16) && (size <= 4096×3072)
        if(((_framePar.width >= 16) && (_framePar.width <= 4096)) && ((_framePar.height >= 16) && (_framePar.height <= 3072)))
        {
            _framePar.frameStart = true;
            _cntFrameHeader++;
        } // END if(((_framePar.width >= 16) && (_framePar.width <= 4096)) && ((_framePar.height >= 16) && (_framePar.height <= 3072)))
        else
        {
            ERROR_MSG("RAWVideoServer: \"Frame size (width, height) corrupted\"");
            ERROR_MSG(_framePar.width);
            ERROR_MSG(_framePar.height);
            _cntFrameHeaderBroken++;
        } // END else
    } // END if( packParCheck(_srvMsgPar, packet->flag, length) )
    else if( _framePar.frameStart && packParCheck(_vidMsgPar, packet->flag, length) )  // Video packet
    {
        const vid_packet * packet = reinterpret_cast<const vid_packet *>(data);
        // Check frame is continous (next_frame_fragment = prev_frame_fragment + 1)
        int next_fragment = packet->fragment_number;
        if((next_fragment > 0) && ((_framePar.currFragment + 1) != next_fragment))
        {
            _framePar.frameStart = false;
            ERROR_MSG("RAWVideoServer: \"Next fragment != (current fragment + 1)\"");
            _cntFragmentBroken++;
            return;
        } // END if((next_fragment > 0) && ((_framePar.currFragment + 1) != next_fragment))
        _framePar.currFragment = next_fragment;
        // Check frame is not out of range
        if(_framePar.currFragment >= _framePar.fragments)
        {
            _framePar.frameStart = false;
            ERROR_MSG("RAWVideoServer: \"Number current fragment >= total fragments\"");
            _cntFragmentBroken++;
            return;
        } // END if(_framePar.currFragment >= _framePar.fragments)
        // Copy received fragment to frame container
        _vidBuf.writeData(packet->data, _vidMsgPar.dataLength);
        // If received frame is ready then send it to process
        if(_framePar.currFragment == (_framePar.fragments - 1))
        {
            saveFrame();
            _cntFrame++;
        } // END if(_framePar.currFragment == (_framePar.fragments - 1))
    } // END else if( _framePar.frameStart && packParCheck(_vidMsgPar, packet->flag, length) )
} // END handle

void RAWVideoServer::saveFramePar(const int32_t& width, const int32_t& heigh, const int32_t& id, const int32_t& frameNumber)
{
    _framePar.number = frameNumber;
    _framePar.width  = width;
    _framePar.height  = heigh;
    _framePar.id     = id;
    _framePar.fragments = (width * heigh) / _vidMsgPar.dataLength;
} // END saveFramePar

void RAWVideoServer::saveFrame()
{
    _vidBuf.setReady();
    _framePar.currFragment = 0;
} // END saveFrame








