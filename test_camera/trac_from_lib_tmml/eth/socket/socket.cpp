#include "socket.h"

void Socket::cls()
{
    if(!is_open()) return;
#ifdef WIN32
    closesocket(_sock);
#else
    close(_sock);
#endif
    _sock = INVALID_SOCKET;
}
void Socket::set_sock(const SOCKET & sock)
{
    this->_sock = sock;
    sockaddr_in address;
    socklen_t address_len = sizeof(address);
    getsockname(sock, reinterpret_cast<sockaddr *>(&address), &address_len);
    _address.set_nip(address.sin_addr.s_addr);
    _address.set_nport(address.sin_port);
}
bool Socket::set_broadcast(const bool & broadcast)
{
    return (setsockopt(_sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char *>(&broadcast), sizeof(broadcast)) == 0);
}
bool Socket::set_reusable(const bool & reusable)
{
    return (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reusable), sizeof(reusable)) == 0);
}
bool Socket::set_non_blocking(const bool & non_blocking)
{
#ifdef WIN32
    DWORD _non_blocking = non_blocking;
    return (ioctlsocket(_sock, FIONBIO, &_non_blocking) == 0);
#else
    int _non_blocking = non_blocking;
    return (fcntl(_sock, F_SETFL, O_NONBLOCK, _non_blocking) == 0);
#endif
}
bool Socket::set_timeout(const int & sec, const int & usec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    return (setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0) &&
           (setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0);
}
bool Socket::set_buffer_size(const int & size)
{
    return (setsockopt(_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&size), sizeof(size)) == 0) &&
           (setsockopt(_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char *>(&size), sizeof(size)) == 0);
}
static bool sockets_initialized = false;
bool Socket::init_sockets()
{
    if(sockets_initialized)
    {
        return true;
    }
#ifdef WIN32
    WSADATA WsaData;
    sockets_initialized = WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
#else
    sockets_initialized = true;
#endif
    return sockets_initialized;
}
void Socket::deinit_sockets()
{
#ifdef WIN32
    if(sockets_initialized)
    {
        WSACleanup();
        sockets_initialized = false;
    }
#endif
}
