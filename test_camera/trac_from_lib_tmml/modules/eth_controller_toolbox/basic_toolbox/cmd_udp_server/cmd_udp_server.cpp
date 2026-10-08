#include "cmd_udp_server.hpp"
#include <thread>
#include <iostream>

#include <bits/stdc++.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

CmdUDPServer::CmdUDPServer(const UDPSrvSettings &_settings, bool &ok) : settings(_settings)
{
    std::thread exec_thr(&CmdUDPServer::exec, this);
    exec_thr.detach();
    std::chrono::milliseconds setup_check_delay(5);
    cmd_bytebuffer = new uint8_t[settings.buf_sz_bytes];
    while(! (sync.failed_setup.load() || sync.success_setup.load()))
    {
        std::this_thread::sleep_for(setup_check_delay);
    }    
    ok = true;
} // -- CmdUDPServer

void CmdUDPServer::exec()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        std::cout << "setsockopt(SO_REUSEADDR) failed" << std::endl;
        sync.failed_setup.store(true);
        return;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
    {
        std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;
        sync.failed_setup.store(true);
        return;
    }
    if (sockfd == -1) 
    {
        std::cout << "socket creation failed..." << std::endl;
        sync.failed_setup.store(true);
        return;
    }
    #ifdef __linux__
    bzero(&servaddr, sizeof(servaddr));
    #endif //__linux__

    servaddr.sin_family         = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr    = inet_addr(settings.ip.c_str());
    servaddr.sin_port           = htons(settings.port);   

    while(true)
    {
        if ((bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr))) < 0) 
        {
            std::cout << "cmd_udp_server::socket bind failed..." << std::endl;
        }
        else
        {
            std::cout 
                << "Socket [ip: " << settings.ip 
                << "; port: " << settings.port 
                << "] successfully binded..\n" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } // -- END while(true) 
    sync.success_setup.store(true);

    sync.execute.store(true);
    struct sockaddr_in cliaddr; // адрес отправителя при получении eth-сообщения 
    socklen_t len_cliaddr = sizeof(cliaddr);  //len is value/result 
    while(sync.execute.load())
    {
        int n_bytes_read = recvfrom(sockfd, cmd_bytebuffer, settings.buf_sz_bytes, 0, ( struct sockaddr *) &cliaddr, 
            &len_cliaddr);
        if(sync.need_quit.load())
        {
            std::cout << "UDP_Server -- success shutdown" << std::endl;
            break;
        }
        if(n_bytes_read > 0)
        {
            std::vector<uint8_t> new_cmd(n_bytes_read);
            std::memcpy(new_cmd.data(), cmd_bytebuffer, n_bytes_read);
            sync.cmd_queue_mtx.lock();
            cmd_queue.push(new_cmd); //TODO: проверить корректность операции, копируется ли вектор.
            sync.cmd_queue_mtx.unlock();
        }
        else if(n_bytes_read == 0)
        {
            if(sync.quit_was_call.load())
            {
                std::cout << "UDPServer: break from rcv exec cycle" << std::endl;
            }
            else
            {
                std::cout << "WARNING: n_bytes_read = 0, close server..." << std::endl;
            }
            break;
        }
        else if(n_bytes_read < 0)
        {

            std::cout << "WARNING: n_bytes_read < 0, close server..." << std::endl;
            break;
        }
    } // -- END while(!sync.need_quit)
    delete[] cmd_bytebuffer;
    sync.exec_complete.store(true);
} // -- END exec

int CmdUDPServer::get_cmd_count()
{
    int bf_sz = -1;
    sync.cmd_queue_mtx.lock();
    bf_sz = cmd_queue.size();
    sync.cmd_queue_mtx.unlock();
    return bf_sz;
} // -- END get_cmd_count

bool CmdUDPServer::pull_one(std::vector<uint8_t> &_cmd)
{
    bool report = false;
    sync.cmd_queue_mtx.lock();
    if(cmd_queue.size() > 0)
    {
        _cmd = std::move(cmd_queue.front());
        cmd_queue.pop();
        report = true;
    }
    else
    {
        report  = false;
    }
    sync.cmd_queue_mtx.unlock();
    return report;
} // -- END pull_one

void CmdUDPServer::quit()
{
    if(sync.quit_was_call.load())
    {
        return;
    }
    sync.quit_was_call.store(true);
    sync.need_quit.store(true);
    shutdown(sockfd, SHUT_RDWR);
    while(!sync.exec_complete.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    close(sockfd);
    sync.success_quit.store(true);
} // -- END quit

CmdUDPServer::~CmdUDPServer()
{
    std::cout << "Destructor CmdUDPServer()" << std::endl;
    if(sync.quit_was_call.load())
    {
        return;
    }
    sync.quit_was_call.store(true);
    sync.need_quit.store(true);
    shutdown(sockfd, SHUT_RDWR); // При отработке деструкторов неопределенность с this_thread::sleep_for
    close(sockfd);
} // -- END ~CmdUDPServer