#include "async_udp_server_simplex_cmd.hpp"

#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 

#if defined(__linux__)
    #include <sys/socket.h> 
    #include <arpa/inet.h> 
    #include <netinet/in.h> 
#elif defined(_WIN32)
    #include <ws2tcpip.h>
#endif // __linux__ || _WIN32

namespace eth_controller::cmd::async::udp::server_simplex
{
    AsyncUDPServerSimplexCmd::AsyncUDPServerSimplexCmd(const AsyncUDPClientSimplexCmdSettings &_settings)
    {
        this->settings = _settings;
        cmd_bytebuffer = new uint8_t[settings.single_command_buf_size_byte];
    } // -- AsyncUDPServerSimplexCmd(const AsyncUDPClientSimplexCmdSettings &settings)

    AsyncUDPServerSimplexCmd::~AsyncUDPServerSimplexCmd()
    {
        close(sockfd);
    } // -- ~AsyncUDPServerSimplexCmd()

    bool AsyncUDPServerSimplexCmd::set_minor_settings(const MinorSettings &_minor_settings)
    {
        if(sync.setup_was_call.load())
        {
            return false;
        }
        minor_settings = _minor_settings;
        return true;
    } // -- END set_minor_settings

    bool AsyncUDPServerSimplexCmd::setup()
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

        #ifdef __linux__
        bzero(&servaddr, sizeof(servaddr));
        #endif //__linux__
        servaddr.sin_family         = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr    = inet_addr(settings.server_ip_v4.c_str());; 
        servaddr.sin_port           = htons(settings.server_port);   

        while(true)
        {
            if ((bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr))) < 0) 
            {
                std::cout << "async_udp_server_simplex::socket bind failed..." << std::endl;
            }
            else
            {
                std::cout << "Socket successfully binded..\n" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } // -- END while(true) 
        sync.exec_receive.store(true);
        sync.success_setup.store(true);
        return true;
    } // -- END setup()

    bool AsyncUDPServerSimplexCmd::exec()
    {
        struct sockaddr_in cliaddr; // адрес отправителя при получении eth-сообщения 
        socklen_t len_cliaddr = sizeof(cliaddr);  //len is value/result 
        if(!sync.success_setup.load())
        {
            std::cout << "call AsyncUDPServerSimplexCmd::exec() but setup not success!" << std::endl;
            return false;
        }
        std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd;
        
        while(sync.exec_receive.load())
        {
            #if defined(__linux__)
            int n_bytes_read = recvfrom(sockfd, cmd_bytebuffer, settings.single_command_buf_size_byte, 0, ( struct sockaddr *) &cliaddr, 
                &len_cliaddr);
            #elif defined(_WIN32)
                int n_bytes_read = recvfrom(sockfd, (char*)cmd_bytebuffer, settings.single_command_buf_size_byte, 0, ( struct sockaddr *) &cliaddr, 
                &len_cliaddr);
            #endif 
            if(n_bytes_read == 0)
            {
                if(!sync.need_quit)
                {
                    std::cout << "WARNING: n_bytes_read = 0" << std::endl;
                    apply_notify_all_handlers(eth_notify_utils::EthNotifyType::ETH_CONNECT_BROKEN_PEER_DISCONNECT);
                }
                else
                {
                    std::cout << "UDP_Server -- success shutdown" << std::endl;
                }
                break;
            }
            else if(n_bytes_read == -1)
            {
                apply_notify_all_handlers(eth_notify_utils::EthNotifyType::ETH_CONNECT_BROKEN_ERROR_READ);
                break;
            }
            else
            {
                // std::cout << "DBG::RcvCmd: ";
                // for(int i = 0; i < n_bytes_read; ++i)
                // {
                //     std::cout << (int)cmd_bytebuffer[i] << " ";
                // }
                // std::cout << std::endl;

                bool receive_report = cmdManager->parse_cmd(cmd_bytebuffer, receive_cmd);
                if(!receive_report)
                {
                    std::cout << "ERROR: cmdManager failed to parse entered cmd" << std::endl;
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
        std::cout << settings.node_alias << " -- AsyncUDPServerSimplexCmd::disconnection done" << std::endl;
        sync.success_quit.store(true);
        std::cout << "END " << settings.node_alias << " AsyncUDPServerSimplexCmd::exec()" << std::endl;
        return true;
    } // -- END exec

    bool AsyncUDPServerSimplexCmd::quit()
    {
        sync.exec_receive.store(false);
        sync.need_quit.store(true);
        #if defined(__linux__)
        shutdown(sockfd, SHUT_RDWR);
        #elif defined(_WIN32)
        shutdown(sockfd, SD_BOTH);
        #endif 
        close(sockfd);
        return true;
    } // -- END quit

    void AsyncUDPServerSimplexCmd::free_buffer()
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

    void AsyncUDPServerSimplexCmd::register_cmd_handler(ICmdHandler *handler)
    {
        cmd_handlers.push_back(handler);
    } // -- END AsyncTcpServerSimplexCmd

    bool AsyncUDPServerSimplexCmd::is_connect_established()
    {
        return sync.success_setup.load();
    } // -- END is_connect_established

    bool AsyncUDPServerSimplexCmd::is_success_quit()
    {
        return sync.success_quit.load();
    } // -- END is_success_quit

    void AsyncUDPServerSimplexCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
    } // -- END register_notify_handler

    void AsyncUDPServerSimplexCmd::apply_notify_all_handlers(eth_notify_utils::EthNotifyType notify_type)
    {
        std::string composite_alias = settings.node_alias + "::ServerSimplex";
        for(auto handler : notify_handlers)
        {
            handler->handle(notify_type, composite_alias);
        }
    } // -- END apply_notify_all_handlers
} // namespace eth_controller::cmd::async::udp::server_simplex
