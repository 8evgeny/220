#include "udp_raw_gray_video_client.hpp"

#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "command_processing/eth_serialization_tools.hpp"


template <typename T> 
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) 
{ 
    os << "[";
    for (int i = 0; i < v.size(); ++i) { 
        os << v[i]; 
        if (i != v.size() - 1) 
            os << ", "; 
    }
    os << "]\n";
    return os; 
}

namespace udp_raw_client_gray
{
    UDP_RAWGray_Client::UDP_RAWGray_Client(const Settings &_settings) : settings(_settings)
    {
        // cmd_bytebuffer = new uint8_t[settings.max_udp_pack_size];
        // valid_init_new_frame_pack_size = 1 + start_frame_pack_data_vec.size();        
    }
    UDP_RAWGray_Client::~UDP_RAWGray_Client()
    {
        std::cout << "BEGIN destructor UDP_RAWGray_Client" << std::endl;;
        if(quit() != CallQuitReport::QUIT_SUCCESS)
        {
            std::cout << "UDP_RAWGray_Client::quit() ERROR!" << std::endl;  
        }
        delete[] cmd_bytebuffer;
        std::cout << "END destructor UDP_RAWGray_Client" << std::endl;;
    }
    CallLaunchReport UDP_RAWGray_Client::launch()
    {
        if(fsm_state.load() == FSMState::STATE_LAUNCH_PROCESS)
        {
            return CallLaunchReport::LAUNCH_PROCESSES_CURRENTLY;
        }
        if(fsm_state.load() == FSMState::STATE_LAUNCH_SUCCESS)
        {
            return CallLaunchReport::LAUNCH_SUCCESS;
        }
        if(fsm_state.load() == FSMState::STATE_LAUNCH_FAILED ||
            fsm_state.load() == FSMState::STATE_QUIT_SUCCESS ||
            fsm_state.load() == FSMState::STATE_QUIT_PROCESS ||
            fsm_state.load() == FSMState::STATE_QUIT_FAILED ||
            fsm_state.load() == FSMState::STATE_WORKFLOW_BREAK)
        {
            return CallLaunchReport::LAUNCH_FAILED;
        }
        
        // доп. защита на случай появления новых состояний
        if(fsm_state.load() != FSMState::STATE_PRIMARY)
        {
            return CallLaunchReport::LAUNCH_FAILED;
        }

        fsm_state.store(FSMState::STATE_LAUNCH_PROCESS);

        cmd_bytebuffer = new uint8_t[settings.max_udp_pack_size];
        valid_init_new_frame_pack_size = 1 + start_frame_pack_data_vec.size() + 4;   

        ts_pipe_frame_buf::Settings ts_pipe_settings;
        ts_pipe_settings.frame_size = cv::Size(settings.frame_width, settings.frame_height);
        ts_pipe_settings.frame_type = CV_8UC1;
        ts_pipe_settings.total_capacity = settings.pipebuf_total_capacity;
        ts_pipe_settings.minCountThresholdOverwritePrepareFrame = 2; // 1 может вызвать разрывы поставки данных, 2 и более сглаживают 

        pipebuf = std::make_shared<ts_pipe_frame_buf::TSPipeFrameBuf>(ts_pipe_settings);
        if(pipebuf == nullptr)
        {
            fsm_state = FSMState::STATE_LAUNCH_FAILED;
            return CallLaunchReport::LAUNCH_FAILED;
        }
        if(!pipebuf->setup())
        {
            fsm_state = FSMState::STATE_LAUNCH_FAILED;
            return CallLaunchReport::LAUNCH_FAILED;
        }
        std::thread exec_thr(&UDP_RAWGray_Client::exec, this);
        exec_thr.detach();
        while(fsm_state.load() == FSMState::STATE_LAUNCH_PROCESS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(check_launch_success_interval_ms));
        }
        if(fsm_state.load() == FSMState::STATE_LAUNCH_SUCCESS)
        {
            return CallLaunchReport::LAUNCH_SUCCESS;
        }
        return CallLaunchReport::LAUNCH_FAILED;
    }

    cv::Mat const * UDP_RAWGray_Client::get_read_frame_ptr()
    {
        return pipebuf->get_ptr_read_frame();
    }

    void UDP_RAWGray_Client::release_frame_ptr(cv::Mat* frame_ptr)
    {
        pipebuf->frame_read_complete(frame_ptr);
    }

    void UDP_RAWGray_Client::exec()
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            std::cout << "setsockopt(SO_REUSEADDR) failed";
        }
        #ifdef SO_REUSEPORT
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
            {
                fsm_state = FSMState::STATE_LAUNCH_FAILED;
                std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;
                return;
            }
        #endif

        if (sockfd == -1) 
        {
            fsm_state = FSMState::STATE_LAUNCH_FAILED;
            std::cout << "socket creation failed..." << std::endl;
            return;
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family    = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr = inet_addr(settings.client_ip_v4.c_str());; 
        servaddr.sin_port = htons(settings.client_port);   

        while(true)
        {
            if ((bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr))) != 0) 
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
        
        fsm_state.store(FSMState::STATE_LAUNCH_SUCCESS);

        struct sockaddr_in cliaddr; // адрес отправителя при получении eth-сообщения 
        socklen_t len_cliaddr = sizeof(cliaddr);  //len is value/result

        


        int s_shift = settings.frame_width * settings.count_rows_in_pack;// single shift cv mat
        int count_full_packs = (int) settings.frame_height / settings.count_rows_in_pack;
        int remainder = settings.frame_height % settings.count_rows_in_pack;
        int last_data_size = remainder * settings.frame_width;
        int count_rows_in_last_frame_pack = remainder;
        bool last_pack_is_partial = false;
        int count_all_packs = count_full_packs;
        if(remainder != 0)
        {
            count_all_packs += 1;
            last_pack_is_partial = true;
        }

        std::vector<bool> received_packages_catalogue(count_all_packs, false);



        set_all_false(received_packages_catalogue);

        cv::Mat* frame_ptr = pipebuf->get_ptr_write_frame();
        if(frame_ptr == nullptr)
        {
            fsm_state.store(FSMState::STATE_WORKFLOW_BREAK);
            return;
        }
        sync.execute.store(true);
        
        while(sync.execute.load())
        {
            // std::cout << "BEFORE recvfrom" << std::endl;
            int n_bytes_read = recvfrom(sockfd, cmd_bytebuffer, settings.max_udp_pack_size, 0, ( struct sockaddr *) &cliaddr, 
                &len_cliaddr);
            // std::cout << "AFTER recvfrom" << std::endl;
            if(n_bytes_read == 0)
            {
                std::cout << "WARNING: n_bytes_read = 0" << std::endl;
                fsm_state.store(FSMState::STATE_WORKFLOW_BREAK);
                pipebuf->frame_write_complete_failure(frame_ptr);
                set_all_false(received_packages_catalogue);
                break;
            }
            else if(n_bytes_read <= -1)
            {
                std::cout << "WARNING: n_bytes_read <= -1" << std::endl;
                fsm_state.store(FSMState::STATE_WORKFLOW_BREAK);
                pipebuf->frame_write_complete_failure(frame_ptr);
                set_all_false(received_packages_catalogue);
                break;
            }
            else
            {
                // проверка на предмет того, что поступивший пакет - сигнал начала отправки нового изображения
                if(check_init_new_frame_pack(n_bytes_read, cmd_bytebuffer))
                {
                    if(check_all_true(received_packages_catalogue))
                    {
                        // std::cout << "Success receive frame" << std::endl;
                        pipebuf->frame_write_complete_success(frame_ptr);
                    }
                    else
                    {
                        // std::cout << "dbg::ER REC catalogue" << received_packages_catalogue << std::endl;
                        // std::cout << "Error receive frame" << std::endl;
                        pipebuf->frame_write_complete_failure(frame_ptr);
                    }
                    set_all_false(received_packages_catalogue);
                    frame_ptr = nullptr;
                    while(true)
                    {
                        frame_ptr = pipebuf->get_ptr_write_frame();
                        if(frame_ptr != nullptr)
                        {
                            break;
                        }
                        else
                        {
                            std::cout << "WARNING: receive pipebuf overflow!" << std::endl;
                        }
                        std::this_thread::sleep_for(std::chrono::microseconds(50));
                    }
                    
                }
                else if(*cmd_bytebuffer == start_byte_frame_data)
                {
                    // продолжение отправки прежнего фрейма
                    uint8_t const* buffer_ptr_tmp = cmd_bytebuffer;
                    ++buffer_ptr_tmp;
                    int32_t rcv_frame_id = eth_deserialization::little_endian::parse_int32(buffer_ptr_tmp);
                    int pack_id = eth_deserialization::little_endian::parse_int32(buffer_ptr_tmp);
                    
                    if(rcv_frame_id != local_frame_id)
                    {
                        std::cout << "ERROR: rcv_frame_id != local_frame_id" << std::endl;
                        continue;
                    }
                    else
                    {
                        // std::cout << "SUCCESS: rcv_frame_id == local_frame_id" << std::endl;
                    }
                    if(pack_id < count_full_packs)
                    {
                        std::memcpy(frame_ptr->data + pack_id * s_shift, buffer_ptr_tmp, s_shift);
                        received_packages_catalogue[pack_id] = true;
                    }
                    else if (pack_id == count_full_packs)
                    {
                        std::memcpy(frame_ptr->data + pack_id * s_shift, buffer_ptr_tmp, last_data_size);
                        received_packages_catalogue[pack_id] = true;
                    }
                }
                else
                {
                    std::cout << "WARNING: Enter no valid data on UDP port = " << (int)(*cmd_bytebuffer) << "..." << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(50)); // оптимизационный момент
            }    
        }
        std::cout << "Close exec cycle" << std::endl;
        pipebuf->frame_write_complete_failure(frame_ptr);
        sync.exec_is_close.store(true);
    }

    CallQuitReport UDP_RAWGray_Client::quit()
    {
        if(fsm_state == FSMState::STATE_QUIT_SUCCESS)
        {
            return CallQuitReport::QUIT_SUCCESS;
        }
        if(fsm_state == FSMState::STATE_QUIT_FAILED)
        {
            return CallQuitReport::QUIT_FAILED;
        }
        if(fsm_state == FSMState::STATE_QUIT_PROCESS)
        {
            return CallQuitReport::QUIT_PROCESS_CURRENTLY;
        }
        if(fsm_state != FSMState::STATE_WORKFLOW_BREAK && 
            fsm_state != FSMState::STATE_LAUNCH_SUCCESS)
        {
            return CallQuitReport::QUIT_FAILED;
        }


        std::cout << "BEGIN UDP_RAWGray_Client::quit()" << std::endl;

        fsm_state.store(FSMState::STATE_QUIT_PROCESS);

        
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        sync.execute.store(false);
        
        int total_check_exec_close_attempts = wait_close_exec_watchdog_time_ms / check_close_exec_interval_ms;
        bool watchdog_time_is_over = true;

        for(int i = 0; i < total_check_exec_close_attempts; ++i)
        {
            if(sync.exec_is_close.load())
            {
                watchdog_time_is_over = false;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(check_close_exec_interval_ms));
        }
        if(watchdog_time_is_over)
        {
            std::cout << "NOTE: UDP_RAWGray_Server - Fail quit: watchdog time is over" << std::endl;
            fsm_state.store(FSMState::STATE_QUIT_FAILED);
            return CallQuitReport::QUIT_FAILED;
        }

        if(!(pipebuf->quit()))
        {
            std::cout << "NOTE: UDP_RAWGray_Server error pipebuf->quit()" << std::endl;
            fsm_state.store(FSMState::STATE_QUIT_FAILED);
            return CallQuitReport::QUIT_FAILED;
        }
        std::cout << "END UDP_RAWGray_Client::quit() -- Success" << std::endl;
        fsm_state.store(FSMState::STATE_QUIT_SUCCESS);
        return CallQuitReport::QUIT_SUCCESS;
    }

    bool UDP_RAWGray_Client::check_init_new_frame_pack(int pack_size, uint8_t const * pack_data)
    {
        if(pack_size != valid_init_new_frame_pack_size)
        {
            return false;
        }
        if(*pack_data != start_byte_init_frame)
        {
            return false;
        }
        ++pack_data;
        for(int i = 0; i < start_frame_pack_data_vec.size(); ++i)
        {
            if((*pack_data) != start_frame_pack_data_vec[i])
            {
                return false;
            }
            ++pack_data;
        }
        local_frame_id = eth_deserialization::little_endian::parse_int32(pack_data);
        return true;
    }
    void UDP_RAWGray_Client::set_all_false(std::vector<bool> &vec)
    {
        for(int i = 0; i < vec.size(); ++i)
        {
            vec[i] = false;
        }
    }
    bool UDP_RAWGray_Client::check_all_true(const std::vector<bool> &vec)
    {
        for(int i = 0; i < vec.size(); ++i)
        {
            if(vec[i] == false)
            {
                return false;
            }
        }
        return true;
    }
}