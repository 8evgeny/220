#include "tcp_client_simplex_base.hpp"

#include "INIReader.h"
#include <thread>
#include <memory>
#include <netinet/in.h>

namespace tcp_client_simplex::base
{
    TCPClientSimplexBase::TCPClientSimplexBase(NetworkSettings settings) : network_settings(settings)
    {

    }

    TCPClientSimplexBase::TCPClientSimplexBase()
    {

    }

    TCPClientSimplexBase::~TCPClientSimplexBase()
    {
        close(sockfd);
    }

    bool TCPClientSimplexBase::quit()
    {
        throw std::runtime_error("Error: not override TCPClientSimplexBase::quit()");
        // close(sockfd);
    }

    bool TCPClientSimplexBase::setup()
    {
        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            std::cout << "setsockopt(SO_REUSEADDR) failed";
        }
        #ifdef SO_REUSEPORT
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
            {
                std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;
                return false;
            }
        #endif

        if (sockfd == -1) {
            std::cout << "socket creation failed..." << std::endl;
            return false;
        }
        else
            std::cout << "Socket successfully created.." << std::endl;

        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(network_settings.server_ip_v4.c_str());
        servaddr.sin_port = htons(network_settings.server_port);
        return true;
    }

    bool TCPClientSimplexBase::try_connect_to_server()
    {
        // connect the client socket to server socket
        if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
        {
            return false;
        }
        else
            printf("success connected to the server..\n");
        return true;
    }


    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        NetworkSettings &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << sectionName << "::base read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "\t INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }

        settings.server_port = reader.GetInteger(sectionName, "dst_port", -1);
        if(settings.server_port == -1)
        {
            std::cout << "\t INIReader parse [" << sectionName << "] : dst_port error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t dst_port = " << settings.server_port << std::endl;
        }

        settings.server_ip_v4 = reader.Get(sectionName, "dst_ip_v4", "oops");
        if(settings.server_ip_v4 == "oops")
        {
            std::cout << "\t INIReader parse [" << sectionName << "] : dst_ip_v4 error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t dst_ip_v4 = " << settings.server_ip_v4 << std::endl;
        }

        std::cout << "END " << sectionName << "::base read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(const NetworkSettings &settings)
    {
        // check valid ipv4 : exist 3 dot, all nums in range [0 .. 255]
        // check valid port (>0) and maybe another tests
        return true;
    }
}
