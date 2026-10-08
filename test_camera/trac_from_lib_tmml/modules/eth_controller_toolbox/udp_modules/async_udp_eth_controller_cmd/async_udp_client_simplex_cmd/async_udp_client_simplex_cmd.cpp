#include "async_udp_client_simplex_cmd.hpp"

#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 

#ifdef __linux__
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#endif // __linux__

namespace eth_controller::cmd::async::udp::client_simplex
{
    AsyncUDPClientSimplexCmd::AsyncUDPClientSimplexCmd(const AsyncUDPClientSimplexCmdSettings &_settings)
    {
        this->settings = _settings;
        cmd_bytebuffer = new uint8_t[settings.single_command_buf_size_byte];
    } // -- AsyncUDPClientSimplexCmd(const AsyncUDPClientSimplexCmdSettings &_settings);

    AsyncUDPClientSimplexCmd::~AsyncUDPClientSimplexCmd()
    {
        close(sockfd);
    } // -- END ~AsyncUDPClientSimplexCmd()

    bool AsyncUDPClientSimplexCmd::set_minor_settings(const MinorSettings &_minor_settings)
    {
        if(sync.setup_was_call.load())
        {
            return false;
        }
        minor_settings = _minor_settings;
        return true;
    } // -- END set_minor_settings

    bool AsyncUDPClientSimplexCmd::setup()
    {
        if(sync.setup_was_call.load())
        {
            return sync.success_setup.load();
        }
        sync.setup_was_call.store(true);

        bool ok_create_cmd_mng = false;
        cmdManager = std::make_shared<eth_cmd_manager::CmdManager>(minor_settings.cmdManager, ok_create_cmd_mng);
        if(!ok_create_cmd_mng)
        {
            return false;
        }

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            std::cout << "setsockopt(SO_REUSEADDR) failed";
            return false;
        }
        // #ifdef SO_REUSEPORT
        //     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        //     {
        //         std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;
        //         return false;
        //     }
        // #endif

        if(sockfd == -1)
        {
            std::cout << "Socket creation failed..." << std::endl;
            return false;
        }
        else
        {
            std::cout << "UDP socket successfully created..." << std::endl;
        }

        #ifdef __linux__
        bzero(&servaddr, sizeof(servaddr));
        #endif // __linux__

        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(settings.server_port); 
        servaddr.sin_addr.s_addr = inet_addr(settings.server_ip_v4.c_str()); 

        sync.execute_tx.store(true);
        sync.success_setup.store(true);
        return true;
    } // -- END setup

    bool AsyncUDPClientSimplexCmd::exec()
    {
        if(!sync.success_setup.load())
        {
            return false;
        }

        while(sync.execute_tx.load())
        {
            if(!cmd_snd_buf.empty())
            {
                std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd_front;

                sync.cmd_snd_buf_mtx.lock();
                cmd_front = cmd_snd_buf.front();
                cmd_snd_buf.pop();
                sync.cmd_snd_buf_mtx.unlock();
                
                int buff_size_tmp;

                // bool serialize_report = cmd_front->serialize_noalloc(cmd_bytebuffer, buff_size_tmp);
                if(!cmdManager->set_cmd(cmd_front))
                {
                    std::cout << "ERROR: cmdManager->set_cmd(cmd_front) return false" << std::endl;
                    continue;
                }
                
                bool serialize_report = cmdManager->serialize_noalloc(cmd_bytebuffer, buff_size_tmp);
                
                // std::cout << "DBG::SndCmd: ";
                // for(int i = 0; i < buff_size_tmp; ++i)
                // {
                //     std::cout << (int)cmd_bytebuffer[i] << " ";
                // }
                // std::cout << std::endl;
                
                

                if(!serialize_report)
                {
                    std::cout << "AsyncUdpClientSimplexCmd::" << settings.node_alias << "send cmd error serialize" << std::endl; 
                }
                else
                {
                    #ifdef _WIN32
                        int bytes = sendto(sockfd, (const char*)cmd_bytebuffer, buff_size_tmp, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                    #elif defined(__linux__)
                        int bytes = sendto(sockfd, cmd_bytebuffer, buff_size_tmp, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                    #endif
                    if(bytes != buff_size_tmp)
                    {
                        // TODO: отправка notify о произошедшем главному классу в иерархии об ошибке - делегирование действий более уполномоченному объекту
                        std::cout << "AsyncUdpClientSimplexCmd: error send cmd" << std::endl;
                    }
                }
            } // -- END if(!cmd_snd_buf.empty())
            // std::cout << "pass" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(settings.cmd_tx_loop_delay_ms));
        } // -- END while(sync.execute_tx.load())
        sync.success_quit.store(true);
        return true;
    } // -- END exec

    bool AsyncUDPClientSimplexCmd::quit()
    {
        sync.execute_tx.store(false);
        close(sockfd);
        return true;
    } // -- END quit

    void AsyncUDPClientSimplexCmd::free_buffer()
    {
        if(sync.success_quit)
        {
            if(!sync.success_free_buffer)
            {
                delete[] cmd_bytebuffer;
                sync.success_free_buffer.store(true);
            }
        }
    } // -- END free_buffer

    // bool AsyncUDPClientSimplexCmd::close_socket(bool force)
    // {
    //     if(sync.success_quit.load() || force)
    //     {
    //         close(sockfd);
    //         return true;
    //     }
    //     return false;
    // }

    bool AsyncUDPClientSimplexCmd::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        sync.cmd_snd_buf_mtx.lock();
        cmd_snd_buf.push(cmd);
        sync.cmd_snd_buf_mtx.unlock();
        return true;
    } // -- END send_cmd

    bool AsyncUDPClientSimplexCmd::is_connect_established()
    {
        return sync.success_setup.load();
    } // -- END is_connect_established

    bool AsyncUDPClientSimplexCmd::is_success_quit()
    {
        return sync.success_quit.load();
    } // -- END is_success_quit

    bool AsyncUDPClientSimplexCmd::is_error_connect()
    {
        return sync.success_quit.load();
    } // -- END is_error_connect

    void AsyncUDPClientSimplexCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
    } // -- END register_notify_handler
} // namespace eth_controller::cmd::async::udp