#include "UDPServer.hpp"
#include "eth/EthGlobal.hpp"
#include "eth/protocols/udp/UDPSocket.hpp"
#include <cassert>
#include <chrono>

UDPServer::UDPServer(const uint32_t& rcvBufSize, const uint32_t& sndBufSize)
    : BaseThread(10)
    , _rcvBufSize{rcvBufSize}
    , _rcvBuf{new uint8_t[_rcvBufSize]}
    , _sndBufSize{sndBufSize}
    , _sndBuf{new uint8_t[_sndBufSize]}
    , _rcvLength{0}
    , _cntFrame{0}
    , _cntFrameLast{0}
    , _fps{0}
    , _timePoint{std::chrono::steady_clock::now()}
{
#ifdef DEBUG
    if(_rcvBufSize <= 4) {
        ERROR_MSG("UDPServer: _rcvBufSize < 4!");
    }
    if(_sndBufSize <= 4) {
        ERROR_MSG("UDPServer: _sndBufSize < 4!");
    }
    if(!_rcvBuf) {
        ERROR_MSG("UDPServer: _rcvBuf not created!");
    }
    if(!_sndBuf) {
        ERROR_MSG("UDPServer: _sndBuf not created!");
    }
#endif
} // END UDPServer

UDPServer::~UDPServer()
{
    delete [] _rcvBuf;
    delete [] _sndBuf;
} // END ~UDPServer

void UDPServer::open(const SocketAddress &srcAddress, const SocketAddress &dstAddress)
{
    _socket.close_();
    _srcAddress = srcAddress;
    _dstAddress = dstAddress;
    startThd();
} // END open

void UDPServer::close()
{
    if(BaseThread::isRunningThd())
    {
        BaseThread::stopThd();
    } // END if(BaseThread::isRunningThd())
    _socket.close_();
    _fps = 0;
    _cntFrame = 0;
} // END close

bool UDPServer::isConnected()
{
   return _socket.isOpen() && isRunningThd();
} // END isConnected

bool UDPServer::send(const uint8_t* data, const int32_t& length)
{
    return _socket.sndto(_dstAddress, data, length);
} // END send

bool UDPServer::initThd()
{
    bool result = true;
    if(_socket.open_(_srcAddress))
    {

        if(!_socket.setNonBlocking(true))
        {
            ERROR_MSG("UDPServer: failed to set UDP socket non blocking");
        } // END if(!_socket.setNonBlocking(true))

        if(!_socket.setBroadcast(true))
        {
            ERROR_MSG("UDPServer: failed to set UDP socket broadcast");
        } // END if(!_socket.setBroadcast(true))

        if(!_socket.setRcvTimeout(1, 0))
        { // only bloking mode
            ERROR_MSG("UDPServer: failed to set UDP socket rcv timeout");
        } // END if(!_socket.setRcvTimeout(1, 0))

        if(!_socket.setSndTimeout(1, 0))
        { // only bloking mode
            ERROR_MSG("UDPServer: failed to set UDP socket snd timeout");
        } // END if(!_socket.setSndTimeout(1, 0))

        if(!_socket.setRcvBufferSize(1920 * 1080 * 2))
        {
            ERROR_MSG("UDPServer: failed to set UDP socket rcv buffer size");
        } // END if(!_socket.setRcvBufferSize(1920 * 1080 * 2))

        if(!_socket.setReusable(true))
        {
            ERROR_MSG("UDPServer: failed to set UDP socket port reusable");
        } // END if(!_socket.setReusable(true))
//        ERROR_MSG("UDPServer: UDP socket buffer: ");
//        bool res = false;/
//        ERROR_MSG(_socket.getRcvBufferSize(res));

    } // END if(_socket.open_(_srcAddress))
    else
    {
        result = false;
        ERROR_MSG("UDPServer: failed open UDP socket");
    } // END if(!_socket.open_(_srcAddress))
    return result;
}

void UDPServer::runThd()
{
    _rcvLength = 0;
    SocketAddress packetSrcAddress;
    do {
        _rcvLength = _socket.rcvfrom(packetSrcAddress, _rcvBuf, _rcvBufSize);
        if(_rcvLength > 0)
        {
            handle(packetSrcAddress, _rcvBuf, _rcvLength);
        } // END if(_rcvLength > 0)
        std::chrono::steady_clock::time_point currentPoint = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>(currentPoint - _timePoint).count() >= 1000)
        {
            _fps = _cntFrame - _cntFrameLast;
            _cntFrameLast = _cntFrame;
            _timePoint = currentPoint;
        } // END if(std::chrono::duration_cast<std::chrono::milliseconds>(currentPoint - _timePoint).count() >= 1000)
    } while (_rcvLength > 0);
} // END runThd

void UDPServer::closeThd()
{
    _socket.close_();
} // END closeThd


