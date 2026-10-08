#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "eth/base/Socket.hpp"

class UDPSocket: public Socket
{
public:
    /**
     *  Constructor
     */
    explicit UDPSocket();
    /** Copy and assignment not allowed */
    UDPSocket(const UDPSocket& ) = delete;
    void operator=(const UDPSocket& ) = delete;
    /**
     * Send byte array of data to socket
     *
     * @param dst        - UDP packet destination address
     * @param data       - Sending data
     * @param dataLength - The size of the sent data [byte]
     * @return           - (byte sent == dataLength) means true
     */
    bool sndto(const SocketAddress& dst, const uint8_t* data, const int32_t& dataLength);
    /**
     * Recive byte array of data from socket
     *
     * @param src     - UDP packet source address
     * @param data    - Array for the received data
     * @param maxSize - Array size [byte]
     * @return        - (byte sent == dataLength) means true
     */
    int32_t rcvfrom(SocketAddress& src, uint8_t* data, const int32_t& maxSize);
};
#endif // UDPSOCKET_H
