#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <stdint.h>

class SocketAddress;
class UDPSocket;

class UDPClient
{
public:
    /**
     *  Constructor
     */
    explicit UDPClient(const uint32_t& sndBufSize);
    /**
     *  Destructor
     */
    virtual ~UDPClient();
    /**
     *  Copy and assignment not allowed
     */
    UDPClient(const UDPClient& ) = delete;
    void operator=(const UDPClient& ) = delete;
    /**
     *  Open the client socket
     */
    bool open(const SocketAddress & srcAddress, const SocketAddress & dstAddress);
    /**
     * Close the socket
     */
    void close();
    /**
     * If the socket is open
     *
     * @return - true if socket is open, else false
     */
    bool isSocketOpen();
    /**
     * Send byte array of data via UDP
     *
     * @param data       - Sent array
     * @param dataLength - The size of the sent array [byte]
     * @return           - (byte sent == dataLength) means true
     */
    bool send(const uint8_t* data, const int32_t& dataLength);

protected:
    /** Where is the data sent from */
    SocketAddress* _srcAddress;
    /** Where is the message sent */
    SocketAddress* _dstAddress;
    /** UDP socket */
    UDPSocket* _socket;
    /** Buffer size [sizeof(uint8_t)] */
    uint32_t _sndBufSize;
    /** Buffer data in the client before sending it to the socket */
    uint8_t* _sndBuf;
};

#endif // UDPCLIENT_H
