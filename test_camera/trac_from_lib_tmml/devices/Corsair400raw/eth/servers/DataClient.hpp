#ifndef DATACLIENT_HPP
#define DATACLIENT_HPP

#include <eth/protocols/udp/UDPClient.hpp>
#include "eth/EthGlobal.hpp"

class DataClient : public UDPClient
{
public:
    /**
     *  Constructor
     */
    explicit DataClient(const uint32_t& sndBufSize,
                        const UDPPacketParametrs& msgPacketPar);
    /**
     *  Desstructor
     */
    ~DataClient();
    /**
     *  Send data with packet header.
     *
     *  @param data    - sending data
     *  @param length  - sending data length [byte]
     *  @return (byte sent == length) means true
     */
    bool sendCmd(const uint8_t* data, const int32_t& length);

private:
    UDPPacketParametrs _msgPar;

};

#endif // DATACLIENT_HPP
