#include "Socket.hpp"

#include "eth/EthGlobal.hpp"
#include "eth/base/SocketAddress.hpp"
#include <string.h>//>

/** API initialization status (true - inited) */
static bool socketsInitialized = false;

Socket::Socket(const int32_t& type, const int32_t& protocol)
    : _sock(INVALID_SOCKET)
    , _socketAddress()
    , _socketType{type}
    , _socketProtocol{protocol}
    , _bindCode{0}
{

} // END Socket

Socket::~Socket()
{
} // END Socket

bool Socket::open_(const SocketAddress& address)
{
    setAddress(address);
    return open_();
} // END open_

bool Socket::open_()
{
    // Create Socket
    bool result = createSocket();
    if(result)
    {
        // Bind Socket to Address
        result = bindSocket();
        if(result)
        {
            result = true;
            LOG_MSG("Socket: socket opened on", " ");
            LOG_MSG(_socketAddress.str_ip(), ":");
            LOG_MSG(_socketAddress.port(), "\n");
        }  // END if(result)
        else
        {
            ERROR_MSG("Socket: failed to bind UDP socket");
            ERROR_MSG(strerror(GETSOCKETERRNO()));
            close_();
        } // END else
    }  // END if(result)
    else
    {
        ERROR_MSG("Socket: failed to create UDP socket");
        ERROR_MSG(strerror(GETSOCKETERRNO()));
    } // END else
    return result;
} // END open_

bool Socket::createSocket()
{
    bool result = false;
    close_();
    _sock = socket(_inetType, _socketType, _socketProtocol);
    if(_sock != INVALID_SOCKET)
    {
        result = true;
    } // END if(_sock != INVALID_SOCKET)
    return result;
} // END createSocket

bool Socket::bindSocket()
{
    bool result = false;
    // Create Address
    sockaddr_in address;
    memset(&address, 0x00, sizeof(sockaddr_in));
    address.sin_family      = _inetType;
    address.sin_addr.s_addr = _socketAddress.nip();
    address.sin_port        = _socketAddress.nport();
    _bindCode = bind(_sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if(_bindCode >= 0)
    {
        result = true;
    } // END if(_bindCode >= 0)
    return result;
} // END bindSocket

void Socket::close_()
{
    if(isOpen())
    {
#ifdef WIN32
        closesocket(_sock);
#elif __unix__ // ! WIN32
        close(_sock);
#endif // END __unix__
        _sock = INVALID_SOCKET;
    } // END if(isOpen())
} // END close_

void Socket::setSock(const SOCKET& sock)
{
    this->_sock = sock;
    sockaddr_in address;
    socklen_t address_len = sizeof(address);
    getsockname(sock, reinterpret_cast<sockaddr*>(&address), &address_len);
    _socketAddress.set_nip(address.sin_addr.s_addr);
    _socketAddress.set_nport(address.sin_port);
} // END setSock

bool Socket::setBroadcast(const bool& broadcast)
{
    return (setsockopt(_sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char *>(&broadcast), sizeof(broadcast)) == 0);
} // END setBroadcast

bool Socket::setReusable(const bool& reusable)
{
    return (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reusable), sizeof(reusable)) == 0);
} // END setReusable

bool Socket::setNonBlocking(const bool& non_blocking)
{
#ifdef WIN32
    DWORD _non_blocking = non_blocking;
    return (ioctlsocket(_sock, FIONBIO, &_non_blocking) == 0);
#elif __unix__ // !WIN32
    int32_t _non_blocking = non_blocking;
    return (fcntl(_sock, F_SETFL, O_NONBLOCK, _non_blocking) == 0);
#endif // !WIN32
}

bool Socket::setRcvTimeout(const int32_t & sec, const int32_t & usec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    const bool result = setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0;
    return result;
} // END setRcvTimeout

bool Socket::setSndTimeout(const int32_t & sec, const int32_t & usec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    const bool result = setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0;
    return result;
} // END setSndTimeout

bool Socket::setRcvBufferSize(const int32_t& size)
{
    return setsockopt(_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&size), sizeof(size)) == 0;
} // END setRcvBufferSize

bool Socket::setSndBufferSize(const int32_t& size)
{
    return setsockopt(_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char *>(&size), sizeof(size)) == 0;
} // END setSndBufferSize

int32_t Socket::getRcvBufferSize(bool& result)
{
    int32_t rcvbuf_size = 0;
#ifdef WIN32
    int32_t len = sizeof(rcvbuf_size);
    result = getsockopt(_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&rcvbuf_size), &len);
#elif __unix__ // !WIN32
    socklen_t len = sizeof(rcvbuf_size);
    result = getsockopt(_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void*>(&rcvbuf_size), &len);
#endif // END WIN32
    return rcvbuf_size;
} // END getRcvBufferSize

int32_t Socket::getSndBufferSize(bool& result)
{
    int32_t sndbuf_size = 0;
#ifdef WIN32
    int32_t len = sizeof(sndbuf_size);
    result = getsockopt(_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&sndbuf_size), &len);
#elif __unix__
    socklen_t len = sizeof(sndbuf_size);
    result = getsockopt(_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void*>(&sndbuf_size), &len);
#endif
    return sndbuf_size;
} // END getSndBufferSize

bool Socket::initSockets()
{
    if(socketsInitialized)
    {
        return true;
    } // END  if(socketsInitialized)
#ifdef WIN32
    WSADATA WsaData;
    socketsInitialized = WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
#elif __unix__ // END WIN32
    socketsInitialized = true;
#endif // END !!WIN32
    return socketsInitialized;
} // END initSockets

void Socket::deinitSockets()
{
#ifdef WIN32
    if(socketsInitialized) {
        WSACleanup();
        socketsInitialized = false;
    }
#elif __unix__ // END WIN32

#endif // END __unix_
} // END deinitSockets




