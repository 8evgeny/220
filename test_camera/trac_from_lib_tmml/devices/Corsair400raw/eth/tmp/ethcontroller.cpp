#include "ethcontroller.h"
#include "servers/RAWVideoDataServer.hpp"

// Host UDP port - 52515
// Device UDP port - 52516

EthController::EthController()
    : _cmdCounter{0}
{
    Socket::initSockets();
    UDPPacketParametrs _msgPar{     UDP_CMD_FLAG, UDP_CMD_LEN, UDP_CMD_SRV_LEN, UDP_CMD_DATA_LEN};
    UDPPacketParametrs _srvMsgPar{  UDP_SRV_FLAG, UDP_SRV_LEN, UDP_SRV_SRV_LEN, UDP_SRV_DATA_LEN};
    UDPPacketParametrs _vidMsgPar{  UDP_VID_FLAG, UDP_VID_LEN, UDP_VID_SRV_LEN, UDP_VID_DATA_LEN};
    _server = new RAWVideoDataServer(_msgPar, _srvMsgPar, _vidMsgPar, CMD_BUF_LEN, FRAME_BUF_LEN);
}

EthController & EthController::inst()
{
    static EthController inst;
    return inst;
}

EthController::~EthController()
{
    delete _server;
    Socket::deinitSockets();
}

bool EthController::start([[maybe_unused]] const std::string & dev,
                          [[maybe_unused]] const std::string & src_mac, const std::string & src_ip, const uint16_t & src_port,
                          [[maybe_unused]] const std::string & dst_mac, const std::string & dst_ip, const uint16_t & dst_port)
{
    bool result = false;
#ifdef PCAP
    if(client->opn(SocketAddress(src_port, src_ip, src_mac, dev), SocketAddress(dst_port, dst_ip, dst_mac, dev)))
    {
        if(server->opn(SocketAddress(src_port, src_ip, src_mac, dev)))
        {
            return true;
        }
        client->cls();
    }
    return false;
#else
    _server->open(SocketAddress(src_port, src_ip), SocketAddress(dst_port, dst_ip));
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    if(_server->isConnected())
    {
        result = true;
    } else {
        _server->close();
    }
    return result;
#endif
}

bool EthController::stop()
{
    _server->close();
    return true;
}

bool EthController::isStarted()
{
    return _server->isConnected();
}

bool EthController::sendCmd(const uint8_t* data, const int32_t& length)
{
    ++_cmdCounter;
    return _server->sendCmd(data, length);
}

uint8_t * EthController::rcvCmd(int32_t& length)
{
    return _server->getMsg(length);
}

uint8_t * EthController::rcvFrame(int32_t& w, int32_t& h, int32_t& id, int32_t& num)
{
    int32_t len = 0;
    FrameParametrs framePar;
    uint8_t* frame{_server->getFrame(len, framePar)};
    w   = framePar.width;
    h   = framePar.height;
    id  = framePar.id;
    num = framePar.number;
    return frame;
}
