#include "async_tcp_client_simplex_cmd.hpp"

#include <netinet/in.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <iostream>
#include <thread>

namespace eth_controller::cmd::async::client_simplex
{
    AsyncTcpClientSimplexCmd::AsyncTcpClientSimplexCmd(Settings _settings) : settings(_settings)
    {
        cmd_bytebuffer = new uint8_t[settings.single_command_buf_size_byte];
    } // -- END AsyncTcpClientSimplexCmd

    AsyncTcpClientSimplexCmd::~AsyncTcpClientSimplexCmd()
    {
        close(sockfd);
    } // -- END AsyncTcpClientSimplexCmd

    bool AsyncTcpClientSimplexCmd::setup()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            std::cout << "setsockopt(SO_REUSEADDR) failed";
            return false;
        }
        #ifdef SO_REUSEPORT
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
            {
                std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;
                return false;
            }
        #endif

        if(sockfd == -1)
        {
            std::cout << "Socket creation failed..." << std::endl;
            return false;
        }
        else
        {
            std::cout << "Socket successfully created..." << std::endl;
        }
        sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(settings.server_ip_v4.c_str());
        servaddr.sin_port = htons(settings.server_port);

        // loop try connect to server
        int attempt_counter = 1;
        while(true)
        {   
            std::cout << "attempt num = " <<  attempt_counter ++ << " try connect to server..." << std::endl;
            if(connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) == 0)
            {
                std::cout << "Success connected to server" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        sync.success_setup.store(true);
        return true;
    } // -- END setup

    bool AsyncTcpClientSimplexCmd::exec() // запуск loop регулярной отправки команд
    {
        if(!sync.success_setup.load())
        {
            return false;
        }

        sync.execute_tx.store(true);

        while(sync.execute_tx.load())
        {
            if(!cmd_snd_buf.empty())
            {
                std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd_front;

                sync.cmd_snd_buf_mtx.lock();
                cmd_front = cmd_snd_buf.front();
                cmd_snd_buf.pop();
                int buff_size_tmp;
                bool serialize_report = cmd_front->serialize_noalloc(cmd_bytebuffer, buff_size_tmp);
                sync.cmd_snd_buf_mtx.unlock();

                if(!serialize_report)
                {
                    std::cout << "AsyncTcpClientSimplexCmd::" << settings.node_alias << "send cmd" << std::endl; 
                }
                else
                {
                    int bytes = send(sockfd, cmd_bytebuffer, buff_size_tmp, 0);
                    if(bytes != buff_size_tmp)
                    {
                        // TODO: отправка notify о произошедшем главному классу в иерархии об ошибке - делегирование действий более уполномоченному объекту
                        std::cout << "AsyncTcpClientSimplexCmd: error send cmd" << std::endl;
                    }
                }
            } // -- END if(!cmd_snd_buf.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(settings.cmd_tx_loop_delay_ms));
        } // -- END while(sync.execute_tx.load())
        sync.success_quit.store(true);
        return true;
    } // -- END exec

    bool AsyncTcpClientSimplexCmd::quit()
    {
        sync.execute_tx.store(false);
        close(sockfd);
        // int try_quit_counter = 0;
        // int max_try = 10;
        // while(!sync.success_quit.load())
        // {
        //     if(try_quit_counter>=max_try)
        //     {
        //         break;
        //     }
        //     ++try_quit_counter;
        //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // }
        // close(sockfd);
        return true;
    } // -- END quit
    
    bool AsyncTcpClientSimplexCmd::close_socket(bool force)
    {
        if(sync.success_quit.load() || force)
        {
            close(sockfd);
            return true;
        }
        return false;
    }

    bool AsyncTcpClientSimplexCmd::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        sync.cmd_snd_buf_mtx.lock();
        cmd_snd_buf.push(cmd);
        sync.cmd_snd_buf_mtx.unlock();
        return true;
    } // -- END send_cmd

    bool AsyncTcpClientSimplexCmd::is_connect_established()
    {
        return sync.success_setup.load();
    } // -- END is_connect_established

    bool AsyncTcpClientSimplexCmd::is_success_quit()
    {
        return sync.success_quit.load();
    } // -- END is_success_quit

    bool AsyncTcpClientSimplexCmd::is_error_connect()
    {
        return sync.success_quit.load();
    } // -- END is_error_connect

    void AsyncTcpClientSimplexCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
    } // -- END register_notify_handler

} // -- namespace eth_controller::cmd::async::client_simplex