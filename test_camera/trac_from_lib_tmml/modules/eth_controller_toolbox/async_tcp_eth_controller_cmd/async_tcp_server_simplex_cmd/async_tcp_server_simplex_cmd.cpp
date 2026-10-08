#include "async_tcp_server_simplex_cmd.hpp"

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
#include <iostream>

namespace eth_controller::cmd::async::server_simplex
{
    AsyncTcpServerSimplexCmd::AsyncTcpServerSimplexCmd(Settings _settings) : settings(_settings)
    {
        cmd_bytebuffer = new uint8_t[settings.single_command_buf_size_byte]; // предварительное выделение буфера с запасом по объему
        eth_cmd_demux = eth_cmd_processing::EthCmdDemux();
    } // -- END AsyncTcpServerSimplexCmd

    AsyncTcpServerSimplexCmd::~AsyncTcpServerSimplexCmd()
    {
        close(sockfd);
        close(connfd);
    } // -- END ~AsyncTcpServerSimplexCmd

    bool AsyncTcpServerSimplexCmd::setup()
    {
        struct sockaddr_in servaddr;
        //socket create and verification
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

        if (sockfd == -1) 
        {
            std::cout << "socket creation failed..." << std::endl;
            return false;
        }
        else
            std::cout << "Socket successfully created..." << std::endl;
        bzero(&servaddr, sizeof(servaddr));
    
        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(settings.server_ip_v4.c_str()); // 127.0.0.1
        servaddr.sin_port = htons(settings.server_port);

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
        {
            std::cout << "Server listening.." << std::endl;
        }

        sockaddr_in cli;
        int len = sizeof(cli);
        socklen_t slen = len;
        connfd = accept(sockfd, (sockaddr*)&cli, &slen);
        if (connfd < 0) {
            std::cout << "server accept failed..." << std::endl;
            return false;
        }
        else
        {
            std::cout << "server accept the client..." << std::endl;
        }
        sync.success_setup.store(true);
        return true;
    } // -- END setup
    
    bool AsyncTcpServerSimplexCmd::exec()
    {
        if(!sync.success_setup.load())
        {
            std::cout << "call AsyncTcpServerSimplexCmd::exec() but setup not success!" << std::endl;
            return false;
        }
        std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd;
        sync.exec_receive.store(true);
        while(sync.exec_receive.load())
        {
            int n_bytes_read = read(connfd, cmd_bytebuffer, settings.single_command_buf_size_byte);

            if(n_bytes_read == 0)
            {
                std::cout << "WARNING: n_bytes_read = 0" << std::endl;
                apply_notify_all_handlers(eth_notify_utils::EthNotifyType::ETH_CONNECT_BROKEN_PEER_DISCONNECT);
                break;
            }
            else if(n_bytes_read == -1)
            {
                apply_notify_all_handlers(eth_notify_utils::EthNotifyType::ETH_CONNECT_BROKEN_ERROR_READ);
                break;
            }
            else
            {
                bool receive_report =  eth_cmd_demux.eth_cmd_demux_to_handler(cmd_bytebuffer, receive_cmd);
                if(!receive_report)
                {
                    std::cout << "attempt incorrect cmd, skip" << std::endl;
                    continue;
                }
                // уведомление подписчиков о вновь поступившей команде

                for(auto handler : cmd_handlers)
                {
                    handler->handle(receive_cmd);
                } 
                
                std::this_thread::sleep_for(std::chrono::microseconds(150)); // оптимизационный момент
            }
        } // -- END while(sync.exec_receive.load())
        std::cout << settings.node_alias << " -- AsyncTcpServerSimplexCmd::disconnection done" << std::endl;
        sync.success_quit.store(true);
        std::cout << "END " << settings.node_alias << " AsyncTcpServerSimplexCmd::exec()" << std::endl;
        return true;
    } // -- END exec

    bool AsyncTcpServerSimplexCmd::quit()
    {
        sync.exec_receive.store(false);
        sync.need_quit.store(true);
        // close_socket(true);
        close(sockfd);
        close(connfd);
        // while(!sync.success_quit.load())
        // {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //     // std::cout << "Try close AsyncTcpServerSimplexCmd" << std::endl;
        // }
        // close(sockfd);
        // close(connfd);
        
        return true;
    } // -- END quit

    bool AsyncTcpServerSimplexCmd::close_socket(bool force)
    {
        if(sync.success_quit.load() || force)
        {
            close(sockfd);
            close(connfd);
            return true;
        }
        return false;
    }

    void AsyncTcpServerSimplexCmd::register_cmd_handler(ICmdHandler *handler)
    {
        cmd_handlers.push_back(handler);
    } // -- END AsyncTcpServerSimplexCmd

    bool AsyncTcpServerSimplexCmd::is_connect_established()
    {
        return sync.success_setup.load();
    } // -- END is_connect_established

    bool AsyncTcpServerSimplexCmd::is_success_quit()
    {
        return sync.success_quit.load();
    } // -- END is_success_quit

    void AsyncTcpServerSimplexCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
    } // -- END register_notify_handler

    void AsyncTcpServerSimplexCmd::apply_notify_all_handlers(eth_notify_utils::EthNotifyType notify_type)
    {
        std::string composite_alias = settings.node_alias + "::ServerSimplex";
        for(auto handler : notify_handlers)
        {
            handler->handle(notify_type, composite_alias);
        }
    } // -- END apply_notify_all_handlers
    
} // namespace eth_controller::cmd::async::server_simplex
