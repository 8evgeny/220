#include "TCPClient.hpp"
#include "eth/base/SocketAddress.hpp"

TCPClient::TCPClient()
    : _serverAddress(new SocketAddress)
{

}

TCPClient::~TCPClient()
{
    delete _serverAddress;
}

bool TCPClient::open([[maybe_unused]]const SocketAddress & self_address, [[maybe_unused]]const SocketAddress & server_address)
{
//    _server_address = server_address;
//    if(Socket::open(self_address))
//    {
//        setBroadcast(true);
//        setRcvTimeout(1, 0);
//        setSndTimeout(1, 0);
//        if(cnct(_server_address)) return true;
//        close();
//        return false;
//    }
    return false;
}


bool TCPClient::send([[maybe_unused]]const void * data, [[maybe_unused]]const int & data_length)
{
//    return snd(data, data_length);
return false;
}
bool TCPClient::send_msg([[maybe_unused]]const std::string & msg)
{
//    return send(msg.data(), msg.length());
return false;
}
