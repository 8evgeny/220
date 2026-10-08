#ifndef SOCKET_H
#define SOCKET_H

#include <cstdint>
#include "eth/base/SocketAddress.hpp"

/*********************************************************
 *  Socket wrapper for win and unix.
 *  For inet family IPv4 (AF_INET)
 ********************************************************/

#ifdef _WIN32
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define GETSOCKETERRNO() (errno)
#endif

class Socket
{
public:
    /**
     *  Constructor
     *
     *  @param type     - Socket type: SOCK_STREAM, SOCK_DGRAM, ...
     *  @param protocol - Socket protocol: IPPROTO_TCP, IPPROTO_UDP, ...
     */
    explicit Socket(const int32_t& type, const int32_t& protocol);
    /**
     *  Destructor
     */
    virtual ~Socket();
    /**
     *  Copy and assignment not allowed
     */
    Socket(const Socket& ) = delete;
    void operator=(const Socket& ) = delete;
    /**
     * Creates a socket and binds it to an address
     *
     * @param address - soket bind address
     * @return - true if socket is open, else false
     */
    bool open_(const SocketAddress& address);
    /**
     * Creates a socket and binds it to an address
     *
     * @return - true if socket is open, else false
     */
    bool open_();
    /**
     * Creates a socket
     *
     * @return - true if socket is creates, else false
     */
    bool createSocket();
    /**
     * Bind socket at the given address
     *
     * @return - true if socket is bind, else false
     */
    bool bindSocket();
    /**
     * Close the socket
     */
    virtual void close_();
    /**
     * If the socket is open
     *
     * @return - true if socket is open, else false
     */
    virtual bool isOpen() const {return (_sock != INVALID_SOCKET);}
    /**
     * Set external socket
     *
     * @param sock - external socket
     */
    void setSock(const SOCKET& sock);
    /**
     * Set the broadcast option for a socket
     *
     * @param broadcast - (true) receive ip broadcast packages
     * @return true if set successful
     */
    bool setBroadcast(const bool& broadcast);
    /**
     * Allows a socket to be forced to bind to a port used by another socket
     *
     * @param reusable - (true) set reuse for socket
     * @return true if set successful
     */
    bool setReusable(const bool& reusable);
    /**
     * Set non blocking for rcv and snd.
     *
     * @param non_blocking - (true) set non blocking mode
     * @return true if set successful
     */
    bool setNonBlocking(const bool& non_blocking);
    /**
     * Set timeout blocking for rcv in blocking mode.
     *
     * @param sec  - second
     * @param usec - micro second
     * @return true if set successful
     */
    bool setRcvTimeout(const int32_t& sec, const int32_t& usec);
    /**
     * Set timeout blocking for snd in blocking mode.
     *
     * @param sec  - second
     * @param usec - micro second
     * @return true if set successful
     */
    bool setSndTimeout(const int32_t & sec, const int32_t & usec);
    /**
     * Set rcv buffer size.
     *
     * @param size  - new buffer size
     * @return true if set successful
     */
    bool setRcvBufferSize(const int32_t& size);
    /**
     * Get rcv buffer size.
     *
     * @param result  - true if set successful
     * @return buffer size
     */
    int32_t getRcvBufferSize(bool& result);
    /**
     * Set snd buffer size.
     *
     * @param size - new buffer size
     * @return true if set successful
     */
    bool setSndBufferSize(const int32_t& size);
    /**
     * Get snd buffer size.
     *
     * @param size - buffer size
     * @return true if set successful
     */
    int32_t getSndBufferSize(bool& size);
    /**
     *  Set new socket address
     *
     *  @param address - new socket address
     */
    void setAddress(const SocketAddress& address) {_socketAddress = address;}
    /**
     *  Get socket address
     *
     *  @return - bind socket address
     */
    const SocketAddress& address() const { return _socketAddress; }

    /**
     * Init socket API (Only once per boot)
     */
    static bool initSockets();
    /**
     * Denit socket API (Only once at closing)
     */
    static void deinitSockets();

protected:
    /** Socket */
    SOCKET _sock;
    /** Socket bind address */
    SocketAddress _socketAddress;
    /** Only IPv4 */
    static constexpr int32_t _inetType{AF_INET};
    /** Socket type */
    const int32_t _socketType;
    /** Socket protocol */
    const int32_t _socketProtocol;
    /** Socket bind code */
    int32_t _bindCode;
};

#endif // SOCKET_H
