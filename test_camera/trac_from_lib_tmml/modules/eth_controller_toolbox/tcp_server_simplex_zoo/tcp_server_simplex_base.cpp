#include "tcp_server_simplex_base.hpp"

#include "INIReader.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

#include <sys/socket.h>
#include <arpa/inet.h>

#include <thread>

// #include <sys/socket.h>
// #include <netdb.h>
// #include <netinet/in.h>

#include <unistd.h>

namespace tcp_server_simplex::base
{
    TCPServerSimplexBase::TCPServerSimplexBase(
        SettingsBase _settings_base) : settings_base(_settings_base)
    {

    } // -- END TCPServerSimplexBase::TCPServerSimplexBase(NetworkSettings settings)

    TCPServerSimplexBase::TCPServerSimplexBase()
    {

    } // -- END TCPServerSimplexBase::TCPServerSimplexBase()

    TCPServerSimplexBase::~TCPServerSimplexBase()
    {
        
        close(sockfd);
    } // -- END TCPServerSimplexBase::~TCPServerSimplexBase()

    bool TCPServerSimplexBase::launch()
    {
        std::cout << "BEGIN " << node_alias << "TCPServerSimplexCmd::exec()" << std::endl;
        // int sockfd;
        struct sockaddr_in servaddr;

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
            std::cout << "Socket successfully created..." << std::endl;
        bzero(&servaddr, sizeof(servaddr));
    
        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(settings_base.net_settings.server_ip_v4.c_str()); // 127.0.0.1
        servaddr.sin_port = htons(settings_base.net_settings.server_port);

        // Binding newly created socket to given IP and verification
        while(true)
        {
            if ((bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) != 0) 
            {
                std::cout << "socket bind failed..." << std::endl;
            }
            else
            {
                std::cout << "Socket successfully binded..\n" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } // -- END while(true)


        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            std::cout << "Listen failed..." << std::endl;
            return false;
        }
        else
            std::cout << "Server listening.." << std::endl;


        sockaddr_in cli;
        int len = sizeof(cli);
        socklen_t slen = len;
        connfd = accept(sockfd, (sockaddr*)&cli, &slen);
        if (connfd < 0) {
            std::cout << "server accept failed..." << std::endl;
            return false;
        }
        else
            std::cout << "server accept the client..." << std::endl;
        sync.accept_success_done.store(true);
        while(!sync.execute_receive.load())
        {
            if(sync.need_quit.load())
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(5));
        }
        if(!this->exec())
        {
            return false;
        }
        return true;
    } // -- END TCPServerSimplexBase::launch()

    bool TCPServerSimplexBase::exec()
    {
        throw std::runtime_error("Error: TCPServerSimplexBase::exec() not override!\n");
    } // -- END TCPServerSimplexBase::exec()

    bool TCPServerSimplexBase::quit()
    {
        throw std::runtime_error("Error: TCPServerSimplexBase::quit() now override!\n");
    } // -- END TCPServerSimplexBase::quit()

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName, 
        SettingsBase &settings_base)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << sectionName << "::server::base read ini net_settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }
        
        settings_base.net_settings.server_port = reader.GetInteger(sectionName, "src_port", -1);
        if(settings_base.net_settings.server_port == -1)
        {
            std::cout << "INIReader parse [" << sectionName << "] : src_port error";
            return false;
        }
        else
        {
            std::cout << "\t src_port = " << settings_base.net_settings.server_port << std::endl;
        }

        settings_base.net_settings.server_ip_v4 = reader.Get(sectionName, "src_ip_v4", "oops");
        if(settings_base.net_settings.server_ip_v4 == "oops")
        {
            std::cout << "INIReader parse [" << sectionName << "] : src_ip_v4 error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t src_ip_v4 = " << settings_base.net_settings.server_ip_v4 << std::endl;
        }

        int enable_notify_handlers_tmp = reader.GetInteger(sectionName, "enable_notify_handlers", -1);
        if(enable_notify_handlers_tmp == -1)
        {
            std::cout << "INIReader parse [" << sectionName << "] : enable_notify_handlers error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t enable_notify_handlers = " << enable_notify_handlers_tmp << std::endl;
            
            std::vector<std::string> valid_notify_modes_vec = {"instant", "regular"};
 
            std::string notify_handlers_mode = reader.Get(sectionName, "notify_handlers_mode", "oops");

            if(std::find(
                valid_notify_modes_vec.begin(), 
                valid_notify_modes_vec.end(),
                notify_handlers_mode) != valid_notify_modes_vec.end())
            {
                std::cout << "\t Notify handlers mode = " << notify_handlers_mode << std::endl;
            }
            else
            {
                std::cout << "Not recognize notify_handlers_mode = \"" << notify_handlers_mode << "\"" << std::endl;
                return false;
            }
            
            if(notify_handlers_mode == "instant")
            {
                settings_base.distrib_settings.notify_handlers.instant.enable = true;
                settings_base.distrib_settings.notify_handlers.regular.enable = false; 
            }
            else if(notify_handlers_mode == "regular")
            {
                settings_base.distrib_settings.notify_handlers.regular.enable = true;
                settings_base.distrib_settings.notify_handlers.instant.enable = false;

                settings_base.distrib_settings.notify_handlers.regular.fps = reader.GetInteger(sectionName, "receive_content_notify_fps", -1);
                if(settings_base.distrib_settings.notify_handlers.regular.fps == -1)
                {
                    std::cout << "INIReader parse [" << sectionName << "] receive_content_notify_fps error" << std::endl;
                    return false;
                }
                else
                {
                    std::cout << "\t receive_content_notify_fps = " << settings_base.distrib_settings.notify_handlers.regular.fps << std::endl;
                }
                
                settings_base.distrib_settings.notify_handlers.regular.buffer_size = reader.GetInteger(sectionName, "receive_buffer_size", -1);
                if(settings_base.distrib_settings.notify_handlers.regular.buffer_size == -1)
                {
                    std::cout << "INIReader parse [" << sectionName << "] receive_buffer_size error" << std::endl; 
                    return false;
                }
                else
                {
                    std::cout << "\t receive_buffer_size = " << settings_base.distrib_settings.notify_handlers.regular.buffer_size << std::endl;
                }
            }
            else
            {
                std::cout << "Not recognize notify_handlers_mode = \"" << notify_handlers_mode << "\"" << std::endl;
                return false;
            }
        }

        std::cout << "END " << sectionName << "::server::base read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(
        const SettingsBase &settings_base)
    {
        // throw ...
        return true;
    }
} // -- namespace tcp_server_simplex::base