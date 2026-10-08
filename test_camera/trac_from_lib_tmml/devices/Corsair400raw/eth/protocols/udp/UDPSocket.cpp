#include "UDPSocket.hpp"
#include "eth/EthGlobal.hpp"
#include "eth/base/SocketAddress.hpp"

#include <cstring>

UDPSocket::UDPSocket()
    : Socket(SOCK_DGRAM, IPPROTO_UDP)
{

} // END UDPSocket

bool UDPSocket::sndto(const SocketAddress& dst, const uint8_t* data, const int32_t& dataLength)
{
    bool result = false;
    if(isOpen())
    {
        // Create Destination Address
        sockaddr_in dstAddress;
        memset(&dstAddress, 0, sizeof(sockaddr_in));

        dstAddress.sin_family      = _inetType;
        dstAddress.sin_addr.s_addr = dst.nip();
        dstAddress.sin_port        = dst.nport();

        // Send message
        int sentBytes = sendto(_sock, reinterpret_cast<const char*>(data), dataLength, 0, reinterpret_cast<sockaddr *>(&dstAddress), sizeof(sockaddr_in));
        if(sentBytes != dataLength)
        {
            ERROR_MSG("UDPSocket: sending error. (sent_bytes != size)");
        } // END if(sentBytes != dataLength)
        else
        {
            result = true;
        } // END else
    } // END  if(isOpen())
    return result;
} // END sndto

int32_t UDPSocket::rcvfrom([[maybe_unused]] SocketAddress& src, uint8_t* data, const int32_t& maxSize)
{
    int32_t rcvBytes = 0;
    if(Socket::isOpen())
    {
        // Packet source address
//        sockaddr_in packetSrcAddress;
//        sockaddr* srcAddress{reinterpret_cast<sockaddr*>(&packetSrcAddress)};
//        socklen_t packetSrcAddressLength = 0;

        // Receive data
        rcvBytes = recvfrom(_sock, reinterpret_cast<char*>(data), maxSize, 0, nullptr/*srcAddress*/,  nullptr/*&packetSrcAddressLength*/);
//        src.set_nip(packetSrcAddress.sin_addr.s_addr);
//        src.set_nport(packetSrcAddress.sin_port);
    } // END if(Socket::isOpen())
    return rcvBytes;
} // END rcvfrom
