#ifndef TCP_CLIENT_SIMPLEX_BASE_HPP
#define TCP_CLIENT_SIMPLEX_BASE_HPP

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

namespace tcp_client_simplex::base
{
    struct NetworkSettings
    {
        std::string server_ip_v4;
        int server_port;
    }; // -- END NetworkSettings

    class TCPClientSimplexBase
    {
    public:
        TCPClientSimplexBase(NetworkSettings settings);
        TCPClientSimplexBase();
        virtual ~TCPClientSimplexBase();
        virtual bool quit();
        bool setup();
        bool try_connect_to_server();

    protected:
        NetworkSettings network_settings;
        sockaddr_in servaddr;
        int sockfd;
    }; // -- END TCPClientSimplexBase

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName, 
        NetworkSettings &settings);
    bool checkValidIniSettings(const NetworkSettings &settings);
}

#endif //TCP_CLIENT_SIMPLEX_BASE_HPP
