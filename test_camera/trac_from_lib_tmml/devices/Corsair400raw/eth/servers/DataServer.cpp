#include "DataServer.hpp"

DataServer::DataServer(const UDPPacketParametrs& msgPacketPar,
                       const uint32_t& dataBufSize)
    : UDPServer()
    , _msgPar(msgPacketPar)
    , _dataBuf(dataBufSize)
{
} // END DataServer

DataServer::~DataServer()
{
    UDPServer::close();
} // END ~DataServer

bool DataServer::sendCmd(const uint8_t* data, const int32_t& length)
{
    cmd_packet* packet = reinterpret_cast<cmd_packet*>(_sndBuf);
    packet->preambule = 0;
    packet->flag = _msgPar.flag;
    std::copy(data, data + length, packet->data);
    return send(_sndBuf, sizeof(cmd_packet) + length);
} // END sendCmd

uint8_t* DataServer::getMsg(int32_t& length)
{
    return _dataBuf.get(length);
} // END getMsg

void DataServer::handle(const SocketAddress& , const uint8_t* data, const uint32_t& length)
{
    const cmd_packet* packet = reinterpret_cast<const cmd_packet *>(data);
    if( (packet->flag == _msgPar.flag) && (length >= _msgPar.length) )  // Command packet
    {
        _dataBuf.writeData(packet->data, _msgPar.dataLength);
        _dataBuf.setReady();
    } // END if( (packet->flag == _msgPar.flag) && (length >= _msgPar.length) )
} // END handle
