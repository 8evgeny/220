#include "cmd_udp_client.hpp"

#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <bits/stdc++.h> 

#include <iostream>


CmdUDPClient::CmdUDPClient(const std::string &dst_ip, int dst_port, bool &ok)
{
    ok = false;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        std::cout << "Socket creation failed..." << std::endl;
        return;
    }
    else
    {
        std::cout << "UDP socket successfully created..." << std::endl;
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(dst_port); 
    servaddr.sin_addr.s_addr = inet_addr(dst_ip.c_str()); 
    ok = true;
} // -- CmdUDPClient

bool CmdUDPClient::snd_cmd(const std::vector<uint8_t> &_cmd)
{
    if(_cmd.empty())
    {
        return false;
    }
    int bytes = sendto(sockfd, _cmd.data(), _cmd.size(), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    if(bytes != _cmd.size())
    {
        return false;
    }
    return true;
} // -- END snd_cmd

CmdUDPClient::~CmdUDPClient()
{
    close(sockfd);
} // -- ~CmdUDPClient