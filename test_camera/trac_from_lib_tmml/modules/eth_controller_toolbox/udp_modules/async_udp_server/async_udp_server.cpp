#include "async_udp_server.hpp"
#include <chrono>
#include <thread>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include "tools/sys_time_extract/sys_time_extract.hpp"
#include "tools/eth_log_tools/eth_log_tools.hpp"

AsyncUDPServer::AsyncUDPServer(const AsyncUDPSrvCfg &_cfg, bool &ok) : cfg(_cfg)
{
    ok = false;
    std::chrono::microseconds setup_check_delay(cfg.setup_check_delay_mcs);
    std::chrono::system_clock::time_point tp_rt; // time point runtime
    std::chrono::system_clock::time_point tp_lim = 
        std::chrono::system_clock::now() + std::chrono::milliseconds(cfg.setup_watchdog_time_ms);
    std::thread exec_thr(&AsyncUDPServer::exec, this);
    exec_thr.detach();
    while(true)
    {
        tp_rt = std::chrono::system_clock::now();
        if(tp_rt > tp_lim)
        {
            ok = false;
            break;
        }
        if(sync.success_setup.load())
        {
            ok = true;
            break;;
        }
        else if(sync.failed_setup.load())
        {
            ok = false;
            return;
        }
        std::this_thread::sleep_for(setup_check_delay);
    } // -- while(true)
} // -- AsyncUDPServer(const AsyncUDPSrvCfg &_cfg, bool &ok)

void AsyncUDPServer::exec()
{
    bool ok_create_cmd_mng = false;
    cmdManager = std::make_shared<eth_cmd_manager::CmdManager>(cfg.cmdManager, ok_create_cmd_mng);
    if(!ok_create_cmd_mng)
    {
        sync.failed_setup.store(true);
        return;
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        std::cout << "ERROR: [AsyncUDPServer] : setsockopt(SO_REUSEADDR) failed";
        sync.failed_setup.store(true);
        return;
    }
#ifdef SO_REUSEPORT
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
    {
        std::cout << "ERROR: [AsyncUDPServer] : setsockopt(SO_REUSEPORT) failed" << std::endl;
        sync.failed_setup.store(true);
        return;
    }
#endif
    if (sockfd == -1) 
    {
        std::cout << "ERROR: [AsyncUDPServer] : socket creation failed..." << std::endl;
        sync.failed_setup.store(true);
        return;
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family         = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr    = inet_addr(cfg.srv_ip.c_str());; 
    servaddr.sin_port           = htons(cfg.srv_port);   

    syst_time_extract::DateTime time_stamp;
    if(cfg.bind_try_mode == BIND_ENDLESS)
    {
        std::cout << "BEGIN [AsyncUDPServer] : endless socket bind loop..." << std::endl;
        while(true)
        {
            time_stamp = syst_time_extract::getCurrentDateTime();
            if ((bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr))) < 0) 
            {
                std::cout << "[" << time_stamp.to_str() << "] : [AsyncUDPServer] : socket bind failed..." << std::endl;
            }
            else
            {
                std::cout << "[" << time_stamp.to_str() << "] : [AsyncUDPServer] Socket " << sockinfo_to_str(cfg.srv_ip, cfg.srv_port) << " successfully bind" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(cfg.bind_try_interval_ms));
        } // -- END while(true) 
    }
    else if(cfg.bind_try_mode == BIND_LIM_ATTEMPTS)
    {
        int cnt = 0;
        while(true)
        {
            if(cnt >= cfg.bind_max_attempts)
            {
                sync.failed_setup.store(true);
                return;
            }
            ++cnt;
            time_stamp = syst_time_extract::getCurrentDateTime();
            if ((bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr))) < 0) 
            {
                std::cout << "[" << time_stamp.to_str() << "] : [AsyncUDPServer] : Socket bind failed..." << std::endl;
            }
            else
            {
                std::cout << "[" << time_stamp.to_str() << "] : [AsyncUDPServer] : Socket " << sockinfo_to_str(cfg.srv_ip, cfg.srv_port) << " successfully bind" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(cfg.bind_try_interval_ms));
        } // -- END while(true)
    }
    else
    {
        std::cout << "ERROR: [AsyncUDPServer] : wrong bind_try_mode" << std::endl;
        sync.failed_setup.store(true);
        return;
    }

    sync.success_setup.store(true);

    uint8_t* cmd_bytebuffer = new uint8_t[cfg.single_command_buf_size_byte];
    struct sockaddr_in cliaddr; // адрес отправителя при получении eth-сообщения (сейчас не используется)
    socklen_t len_cliaddr = sizeof(cliaddr); 
    std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd;
    std::chrono::microseconds cmd_rx_loop_delay(cfg.cmd_rx_loop_delay_mcs);
    sync.execute.store(true);
    while(sync.execute.load())
    {
        int n_bytes_read = recvfrom(sockfd, cmd_bytebuffer, cfg.single_command_buf_size_byte, 0, ( struct sockaddr *) &cliaddr, 
            &len_cliaddr);
        if(n_bytes_read > 0)
        {
            bool receive_report = cmdManager->parse_cmd(cmd_bytebuffer, receive_cmd);
            if(!receive_report)
            {
                std::cout << "ERROR: [AsyncUDPServer]: cmdManager failed to parse entered cmd" << std::endl;
                continue;
            }
            sync.cmd_snd_buf_mtx.lock();
            cmd_rcv_buf.push(receive_cmd);
            sync.cmd_snd_buf_mtx.unlock();
        }
        else if(n_bytes_read == 0)
        {
            if(!sync.execute)
            {
                std::cout << "WARNING: n_bytes_read = 0" << std::endl;
            }
            else
            {
                std::cout << "UDP_Server -- success shutdown" << std::endl;
            }
            break;
        }
        else if(n_bytes_read == -1)
        {
            std::cout << "ERROR: n_bytes_read < 0" << std::endl;
            break;
        }
        std::this_thread::sleep_for(cmd_rx_loop_delay); // оптимизационный момент
    } // -- END while(sync.exec_receive.load())
    close(sockfd);
    delete[] cmd_bytebuffer;
    sync.exec_complete.store(true);
} // -- END exec

int AsyncUDPServer::get_cmd_count()
{
    int count = -1;
    sync.cmd_snd_buf_mtx.lock();    
    count = cmd_rcv_buf.size();
    sync.cmd_snd_buf_mtx.unlock();
    return count;    
} // -- END get_cmd_count 

bool AsyncUDPServer::pull_one(std::shared_ptr<eth_cmd_processing::EthCmdBase> &cmd)
{
    bool rep;
    sync.cmd_snd_buf_mtx.lock();    
    if(cmd_rcv_buf.size() > 0)
    {
        cmd = cmd_rcv_buf.front();
        cmd_rcv_buf.pop();
        rep = true;
    }
    else
    {
        cmd = nullptr;
        rep = false;
    }
    sync.cmd_snd_buf_mtx.unlock();
    return rep;
} // -- END pull_one

bool AsyncUDPServer::quit()
{
    if(sync.quit_was_call.load())
    {
        return sync.success_quit.load();
    }
    sync.quit_was_call.store(true);
    sync.execute.store(false);
    shutdown(sockfd, SHUT_RDWR);
    std::chrono::system_clock::time_point tp_lim = 
        std::chrono::system_clock::now() + std::chrono::milliseconds(cfg.quit_watchdog_time_ms);
    std::chrono::system_clock::time_point tp_rt; // time point runtime
    std::chrono::milliseconds quit_check_delay(cfg.quit_check_delay_ms);
    bool correct_quit = true;
    while(!sync.exec_complete.load())
    {
        tp_rt = std::chrono::system_clock::now();
        if(tp_rt > tp_lim)
        {
            correct_quit = false;
            break;
        }
        std::this_thread::sleep_for(quit_check_delay);
    }
    sync.success_quit.store(correct_quit);
    return correct_quit;
} // -- END quit

AsyncUDPServer::~AsyncUDPServer()
{
    quit();
} // -- END ~AsyncUDPServer()