#include "async_udp_client.hpp"
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

AsyncUDPClient::AsyncUDPClient(const AsyncUDPCliCfg &_cfg, bool &ok) : cfg(_cfg)
{
    ok = false;
    std::chrono::microseconds setup_check_delay(cfg.setup_check_delay_mcs);
    std::chrono::system_clock::time_point tp_rt; // time point runtime
    std::chrono::system_clock::time_point tp_lim = 
        std::chrono::system_clock::now() + std::chrono::milliseconds(cfg.setup_watchdog_time_ms);
    std::thread exec_thr(&AsyncUDPClient::exec, this);
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
            return;
        }
        else if(sync.failed_setup.load())
        {
            ok = false;
            return;
        }
        std::this_thread::sleep_for(setup_check_delay);
    } // -- while(true)
} // -- AsyncUDPClient(const AsyncUDPCliCfg &_cfg, bool &ok)

void AsyncUDPClient::exec()
{
    bool ok_create_cmd_mng = false;
    
    cmdManager = std::make_shared<eth_cmd_manager::CmdManager>(cfg.cmdManager, ok_create_cmd_mng);
    if(!ok_create_cmd_mng)
    {
        sync.failed_setup.store(true);
        return;
    }
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        std::cout << "ERROR: [AsyncUDPClient] : setsockopt(SO_REUSEADDR) failed";
        sync.failed_setup.store(true);
        return;
    }
    if(sockfd == -1)
    {
        std::cout << "ERROR: [AsyncUDPClient] : Socket creation failed..." << std::endl;
        sync.failed_setup.store(true);
        return;
    }
    else
    {
        std::cout << "ERROR: [AsyncUDPClient] : UDP socket successfully created..." << std::endl;
    }
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(cfg.dst_port); 
    servaddr.sin_addr.s_addr = inet_addr(cfg.dst_ip.c_str()); 

    uint8_t* cmd_bytebuffer = new uint8_t[cfg.single_command_buf_size_byte];

    sync.success_setup.store(true); // SUCCESS SETUP ! (need for constructor)

    std::chrono::microseconds cmd_tx_loop_delay(cfg.cmd_tx_loop_delay_mcs), residual_delay,exec_one_duration;
    std::chrono::system_clock::time_point tp_begin, tp_end;
    tp_begin = std::chrono::system_clock::now(); 

    bool serialize_report;
    int buff_size_tmp;
    std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd_front;

    sync.execute.store(true);
    while(sync.execute.load())
    {
        if(!cmd_snd_buf.empty())
        {
            sync.cmd_snd_buf_mtx.lock();
            cmd_front = cmd_snd_buf.front();
            cmd_snd_buf.pop();
            sync.cmd_snd_buf_mtx.unlock();

            if(!cmdManager->set_cmd(cmd_front))
            {
                std::cout << "ERROR: cmdManager->set_cmd(cmd_front) return false" << std::endl;
                continue;
            }
            serialize_report = cmdManager->serialize_noalloc(cmd_bytebuffer, buff_size_tmp);
            if(serialize_report)
            {
                int bytes = sendto(sockfd, cmd_bytebuffer, buff_size_tmp, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                if(bytes != buff_size_tmp)
                {
                    // TODO: отправка notify о произошедшем главному классу в иерархии об ошибке - делегирование действий более уполномоченному объекту
                    std::cout << "AsyncUdpClientSimplexCmd: error send cmd" << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR: [AsyncUDPClient] : send cmd error serialize" << std::endl; 
            }
        } // -- END if(!cmd_snd_buf.empty())

        tp_end = std::chrono::system_clock::now(); 
        exec_one_duration = std::chrono::duration_cast<std::chrono::microseconds>(tp_end - tp_begin);
        residual_delay = cmd_tx_loop_delay - exec_one_duration;
        if(residual_delay.count() > 0) // защита от tp_end == tp_begin (быстропротекающий процесс), т.к. может вызвать блокировку потока
        {
            std::this_thread::sleep_for(residual_delay);
        }
        tp_begin = tp_end;
    } // -- while(sync.execute.load())
    
    close(sockfd);
    delete[] cmd_bytebuffer;
    sync.exec_complete.store(true);
} // -- END exec

bool AsyncUDPClient::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
{
    bool report = true;
    sync.cmd_snd_buf_mtx.lock();
    if(cmd_snd_buf.size() > 0)
    {
        // проверка на отправку одного и того же shared-указателя - такое недопустимо
        eth_cmd_processing::EthCmdBase* raw_back_ptr = cmd_snd_buf.back().get();
        if(raw_back_ptr == cmd.get())
        {
            report = false;
            sync.cmd_snd_buf_mtx.unlock();
            return report;
        }
    }
    cmd_snd_buf.push(cmd);
    report = true;
    sync.cmd_snd_buf_mtx.unlock();
    return report; // maybe return false if cmd_snd_buf.size() > threshold ... (TODO)
} // -- END send_cmd

bool AsyncUDPClient::quit()
{
    if(sync.quit_was_call.load())
    {
        return sync.success_quit.load();
    }
    sync.quit_was_call.store(true);
    sync.execute.store(false);

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

AsyncUDPClient::~AsyncUDPClient()
{
    quit();
} // ~AsyncUDPClient()