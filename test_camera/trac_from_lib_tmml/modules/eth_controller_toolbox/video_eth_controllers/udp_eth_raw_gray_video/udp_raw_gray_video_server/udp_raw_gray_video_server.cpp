#include "udp_raw_gray_video_server.hpp"
#include <iostream>

#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <thread>
#include <iostream>

#include <cassert>
#include <cstdint>

#include "command_processing/eth_serialization_tools.hpp"

namespace udp_raw_server_gray
{
    UDP_RAWGray_Server::UDP_RAWGray_Server(const Settings &_settings) : settings(_settings) 
    {
        // cmd_bytebuffer = new uint8_t[settings.max_udp_pack_size];
        // tp_watchdog_push_delay = std::chrono::high_resolution_clock::now();
        // push_frame_min_delay_mcs = 1e6 / settings.max_fps_push_buffer_frames;
    }
    UDP_RAWGray_Server::~UDP_RAWGray_Server()
    {
        std::cout << "BEGIN destructor UDP_RAWGray_Server" << std::endl;
        if(quit() != CallQuitReport::QUIT_SUCCESS)
        {
            std::cout << "UDP_RAWGray_Server::quit() ERROR!" << std::endl;  
        }
        delete[] cmd_bytebuffer;
        std::cout << "END destructor UDP_RAWGray_Server" << std::endl;
    }

    CallLaunchReport UDP_RAWGray_Server::launch()
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
        // setup pipebuf
        tp_real_fps = std::chrono::high_resolution_clock::now();

        cmd_bytebuffer = new uint8_t[settings.max_udp_pack_size];
        tp_watchdog_push_delay = std::chrono::high_resolution_clock::now();
        push_frame_min_delay_mcs = 1e6 / settings.max_fps_push_buffer_frames;
        
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
        std::thread exec_thr(&UDP_RAWGray_Server::exec, this);
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
    } // -- END launch
    
    CallSendFrameReport UDP_RAWGray_Server::send_frame(const cv::Mat &frame)
    {
        if(fsm_state.load() != FSMState::STATE_LAUNCH_SUCCESS)
        {
            return CallSendFrameReport::SEND_FRAME_ERROR;
        }
        if(frame.empty())
        {
            return CallSendFrameReport::SEND_FRAME_ERROR;
        }
        std::chrono::system_clock::time_point tp_now = std::chrono::high_resolution_clock::now();
        if(std::chrono::duration_cast<std::chrono::microseconds>(
            tp_now - tp_watchdog_push_delay).count() < push_frame_min_delay_mcs)
        {
            return CallSendFrameReport::SEND_FRAME_SKIP_MAX_REQUEST_RATE_EXCEED;
        }

        sync.snd_mtx.lock();
        tp_watchdog_push_delay = tp_now;

        CallSendFrameReport report;

        if(pipebuf->get_count_prepared() == settings.pipebuf_total_capacity)
        {
            report = CallSendFrameReport::SEND_FRAME_SUCCESS_OVERWRITE_QUEUE_HEAD;
        }
        else
        {
            report = CallSendFrameReport::SEND_FRAME_SUCCESS_PUSH_FRAME_QUEUE_TAIL;
        }

        cv::Mat* frame_ptr = pipebuf->get_ptr_write_frame();
        if(frame_ptr == nullptr)
        {
            sync.snd_mtx.unlock();
            return CallSendFrameReport::SEND_FRAME_ERROR;
        }

        cv::Mat frame_prepared;
        if(frame.type() != CV_8UC1 || frame.size() != cv::Size(settings.frame_width, settings.frame_height))
        {
            cv::Mat frame_process1;
            if(frame.type() != CV_8UC1)
            {
                if(frame.type() != CV_8UC3)
                {
                    pipebuf->frame_write_complete_failure(frame_ptr);
                    sync.snd_mtx.unlock();
                    return CallSendFrameReport::SEND_FRAME_ERROR;
                }
                cv::cvtColor(frame, frame_process1, cv::ColorConversionCodes::COLOR_BGR2GRAY);
            }
            else
            {
                frame_process1 = frame;
            }
            cv::Mat frame_process2;
            if(frame.size() != cv::Size(settings.frame_width, settings.frame_height))
            {
                cv::resize(frame_process1, frame_process2, cv::Size(settings.frame_width, settings.frame_height));
            }
            else
            {
                frame_process2 = frame_process1;
            }

            frame_prepared = frame_process1;
        }
        else
        {
            frame_prepared = frame;
        }

        static size_t n_bytes_prepared_frame = settings.frame_width * settings.frame_height; 
        if(frame.isContinuous())
        {
            std::memcpy(frame_ptr->data, frame_prepared.data, n_bytes_prepared_frame); // faster than copyTo
        }
        else
        {
            frame_prepared.copyTo(*frame_ptr);
        }
        pipebuf->frame_write_complete_success(frame_ptr);
        sync.snd_mtx.unlock();
        return report;
    } // -- END send_frame

    CallQuitReport UDP_RAWGray_Server::quit()
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

        std::cout << "BEGIN UDP_RAWGray_Server::quit()" << std::endl;

        fsm_state.store(FSMState::STATE_QUIT_PROCESS);
        close(sockfd);
        sync.execute.store(false);
        
        int total_check_exec_close_attempts = wait_close_exec_watchdog_time_ms / check_close_exec_interval_ms;
        bool watchdog_time_is_over = true;

        for(int i = 0; i < total_check_exec_close_attempts; ++i)
        {
            if(sync.exec_is_close.load())
            {
                std::cout << "Success wait exec cycle close" << std::endl;
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

        if(!pipebuf->quit())
        {
            std::cout << "NOTE: UDP_RAWGray_Server error pipebuf->quit()" << std::endl;
            fsm_state.store(FSMState::STATE_QUIT_FAILED);
            return CallQuitReport::QUIT_FAILED;
        }

        std::cout << "END UDP_RAWGray_Server::quit() -- Success" << std::endl;
        
        fsm_state.store(FSMState::STATE_QUIT_SUCCESS);
        return CallQuitReport::QUIT_SUCCESS;
    }

    void UDP_RAWGray_Server::exec()
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            fsm_state.store(FSMState::STATE_LAUNCH_FAILED);
            std::cout << "setsockopt(SO_REUSEADDR) failed" << std::endl;
            return;
        }
        #ifdef SO_REUSEPORT
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
            {
                fsm_state.store(FSMState::STATE_LAUNCH_FAILED);
                std::cout << "setsockopt(SO_REUSEPORT) failed" << std::endl;
                return;
            }
        #endif

        if(sockfd == -1)
        {
            fsm_state.store(FSMState::STATE_LAUNCH_FAILED);
            std::cout << "Socket creation failed..." << std::endl;
            return;
        }
        else
        {
            std::cout << "UDP socket successfully created..." << std::endl;
        }
        
        bzero(&servaddr, sizeof(servaddr));

        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(settings.client_port); 
        servaddr.sin_addr.s_addr = inet_addr(settings.client_ip_v4.c_str()); 

        fsm_state.store(FSMState::STATE_LAUNCH_SUCCESS);
        sync.execute.store(true);
        cv::Mat* frame_ptr;
        std::chrono::microseconds cross_frame_delay(settings.cross_frame_interval_mcs);
        std::chrono::microseconds cross_frame_check_buffer_empty_delay(settings.cross_frame_interval_check_buffer_empty_mcs);
        while(sync.execute.load())
        {
            if(pipebuf->get_count_prepared() > 0)
            {
                // std::cout << "BEGIN send frame" << std::endl;
                SFDReport rep = send_frame_direct();
                if(rep != SFDReport::SFDR_SUCCESS)
                {
                    std::cout << "rep != SFDReport::SFDR_SUCCESS" << std::endl;
                }

                ++frame_cnt_local;
                if(frame_cnt_local == max_frame_cnt_local)
                {
                    frame_cnt_local = 0;
                }

                inst_tp_real_fps = std::chrono::high_resolution_clock::now();
                inst_real_stream_single_duration_mcs = 
                    std::chrono::duration_cast<std::chrono::microseconds>(inst_tp_real_fps - tp_real_fps).count();
                tp_real_fps = inst_tp_real_fps;
                inst_real_stream_fps = 1e6 / inst_real_stream_single_duration_mcs;
                std::cout << "inst_real_stream_fps = " << inst_real_stream_fps << std::endl;
                ++total_send_frames_by_session;
                total_stream_exec_time_mcs += inst_real_stream_single_duration_mcs;
                std::cout << "AVERAGE_FPS = " << 1e6 * total_send_frames_by_session / total_stream_exec_time_mcs << std::endl;

                // std::cout << "END send frame" << std::endl;
                if(rep == SFDReport::SFDR_PIPEBUF_EMPTY)
                {
                    continue;
                }
                if(rep == SFDReport::SFDR_ETH_CONNECTION_ERROR)
                {
                    fsm_state.store(FSMState::STATE_WORKFLOW_BREAK);
                    sync.execute.store(false);
                    break;
                }
            }
            else
            {
                std::this_thread::sleep_for(cross_frame_check_buffer_empty_delay);
            }
            std::this_thread::sleep_for(cross_frame_delay);
        } // -- END while(sync.execute.load())
        sync.exec_is_close.store(true);
    } // -- END exec

    UDP_RAWGray_Server::SFDReport UDP_RAWGray_Server::send_frame_direct()
    {
        static std::chrono::microseconds cross_package_delay(settings.cross_package_interval_mcs);
        cv::Mat const * frame_ptr;
        frame_ptr = pipebuf->get_ptr_read_frame();
        if(frame_ptr == nullptr)
        {
            return SFDReport::SFDR_PIPEBUF_EMPTY;  
        }

        // отправка начального пакета, сигнализирующего о том, что начата отправка кадра (расщепленного на UDP пакеты)
        uint8_t* buffer_tmp_ptr = cmd_bytebuffer;
        
        *buffer_tmp_ptr = start_byte_init_frame; ++buffer_tmp_ptr;
        std::memcpy(buffer_tmp_ptr, start_frame_pack_data_vec.data(), start_frame_pack_data_vec.size());
        buffer_tmp_ptr+= start_frame_pack_data_vec.size();
        eth_serialization::little_endian::conv2byteArrPushBack_int32t(frame_cnt_local, buffer_tmp_ptr);

        int init_pack_size = 1 + start_frame_pack_data_vec.size() + 4; // start_byte, init_pack, frame_id

        // std::cout << "dbg:: ";
        // for(int i = 0; i < 15; ++i)
        // {
        //     std::cout << (int)cmd_bytebuffer[i] << " ";
        // }
        // std::cout << std::endl;
        int bytes = sendto(sockfd, cmd_bytebuffer, init_pack_size, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        if(bytes != init_pack_size)
        {
            // TODO: отправка notify о произошедшем главному классу в иерархии об ошибке - делегирование действий более уполномоченному объекту
            std::cout << "AsyncTcpClientSimplexCmd: error send cmd" << std::endl;
            return SFDReport::SFDR_ETH_CONNECTION_ERROR;
        }

        

        int s_shift = settings.frame_width * settings.count_rows_in_pack;// single shift cv mat
        int count_full_packs = (int) settings.frame_height / settings.count_rows_in_pack;
        int remainder = settings.frame_height % settings.count_rows_in_pack;
        int last_data_size = remainder * settings.frame_width;
        int count_rows_in_last_frame_pack = remainder;
        bool last_pack_is_partial = false;
        if(remainder != 0)
        {
            last_pack_is_partial = true;
        }
        uint32_t* pack_cnt_u32ptr;
        uint32_t pack_cnt_u32;
        uint8_t* frame_data_ptr = frame_ptr->data;

        int ordinary_frame_pack_size = 1 + 4 + 4 + s_shift; // start_byte, frame_id, pack_id, data (n * rows)
        int last_frame_pack_size = 1 + 4 + last_data_size;

        for(int i =0; i < count_full_packs; ++i)
        {
            buffer_tmp_ptr = cmd_bytebuffer;
            *buffer_tmp_ptr = start_byte_frame_data; ++buffer_tmp_ptr;

            eth_serialization::little_endian::conv2byteArrPushBack_int32t(frame_cnt_local, buffer_tmp_ptr);
            eth_serialization::little_endian::conv2byteArrPushBack_int32t(i, buffer_tmp_ptr);
            
            std::memcpy(buffer_tmp_ptr, frame_data_ptr, s_shift); buffer_tmp_ptr += s_shift;
            frame_data_ptr += s_shift;

            // std::cout << "dbg:: ";
            // for(int i = 0; i < 15; ++i)
            // {
            //     std::cout << (int)cmd_bytebuffer[i] << " ";
            // }
            // std::cout << std::endl;

            int bytes = sendto(sockfd, cmd_bytebuffer, ordinary_frame_pack_size, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
            if(bytes != ordinary_frame_pack_size)
            {
                // TODO: отправка notify о произошедшем главному классу в иерархии об ошибке - делегирование действий более уполномоченному объекту
                std::cout << "AsyncTcpClientSimplexCmd: error send cmd" << std::endl;
                pipebuf->frame_read_complete(frame_ptr);
                return SFDReport::SFDR_ETH_CONNECTION_ERROR;
            }
            std::this_thread::sleep_for(cross_package_delay);
        }

        if(last_pack_is_partial)
        {
            buffer_tmp_ptr = cmd_bytebuffer;
            *buffer_tmp_ptr = start_byte_frame_data; ++buffer_tmp_ptr;
            eth_serialization::little_endian::conv2byteArrPushBack_int32t(frame_cnt_local, buffer_tmp_ptr);
            eth_serialization::little_endian::conv2byteArrPushBack_int32t(count_full_packs, buffer_tmp_ptr);
            std::memcpy(buffer_tmp_ptr, frame_data_ptr, last_data_size); buffer_tmp_ptr += last_data_size;

            int bytes = sendto(sockfd, cmd_bytebuffer, last_frame_pack_size, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
            if(bytes != last_frame_pack_size)
            {
                // TODO: отправка notify о произошедшем главному классу в иерархии об ошибке - делегирование действий более уполномоченному объекту
                std::cout << "AsyncTcpClientSimplexCmd: error send cmd" << std::endl;
                pipebuf->frame_read_complete(frame_ptr);
                return SFDReport::SFDR_ETH_CONNECTION_ERROR;
            }
        }

        pipebuf->frame_read_complete(frame_ptr);

        
        return SFDReport::SFDR_SUCCESS;
    } // -- END send_frame_direct
}
