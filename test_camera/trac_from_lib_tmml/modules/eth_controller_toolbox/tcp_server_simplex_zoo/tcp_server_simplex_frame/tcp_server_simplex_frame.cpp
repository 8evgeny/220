#include "tcp_server_simplex_frame.hpp"

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

#include <sys/socket.h>
#include <arpa/inet.h>

#include <opencv2/opencv.hpp>
#include <thread>
#include "INIReader.h"

#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <iostream>

namespace chrono = std::chrono;

namespace tcp_server_simplex::frame
{
    TCPServerSimplexFrame::TCPServerSimplexFrame(
        RXTX_FrameSettings rxtx_settings, 
        tcp_server_simplex::base::SettingsBase _settings_base) : rxtx_settings(rxtx_settings) 
    {
        this->settings_base = _settings_base;
#if defined(CCM_8UC1)
        frame_receive = cv::Mat(rxtx_settings.receive_frame_size, CV_8UC1);
#elif defined(CCM_8UC3)
        frame_receive = cv::Mat(rxtx_settings.receive_frame_size, CV_8UC3);
#else
        throw std::runtime_error("TCPServerSimplex -- not support current frame channels size");
#endif
    frame_ptr = frame_receive.data;
    rc_frame_data_row_len = frame_receive.total() * frame_receive.elemSize(); // длина пакета изображения в байтах
    package_buffer_len = rc_frame_data_row_len + preambule_len;
    frame_buffer = new uint8_t[rc_frame_data_row_len];

    } // -- END TCPServerSimplexFrame::TCPServerSimplexFrame(RXTX_FrameSettings rxtx_settings)

    TCPServerSimplexFrame::TCPServerSimplexFrame()
    {

    } // -- END TCPServerSimplexFrame::TCPServerSimplexFrame()

    TCPServerSimplexFrame::~TCPServerSimplexFrame()
    {
        delete[] frame_buffer;
        this->TCPServerSimplexBase::~TCPServerSimplexBase();
    } // -- END TCPServerSimplexFrame::~TCPServerSimplexFrame()
    
    bool TCPServerSimplexFrame::exec()
    {
        std::cout << "BEGIN " << node_alias << " TCPServerSimplexFrame::exec()" << std::endl;

        // cv::Mat test_mat;
        // int counter = 0;
        // char buff_tmp[9] = "receive\n";

        while(sync.execute_receive.load())
        {
            // std::cout << " ==== ITERATION SERVER EXEC RECEIVE" << std::endl;
            // if(!is_connected(connfd))
            // {
            //     break;
            // }
            //std::cout << counter++ << std::endl;
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            //wait_receive_frame({1000,700}, 1, test_mat);
            //cv::imshow("win1", test_mat);
            //cv::waitKey(1);
        }
        std::cout << node_alias << " -- SimplexServer::disconnection done" << std::endl;
        close(sockfd);
        std::cout << node_alias << "TCPServerSimplexFrame socket success closed" << std::endl;
        sync.exec_quit_success.store(true);
        std::cout << "END " << node_alias << " TCPServerSimplexFrame::exec()" << std::endl;
        return true;
    } // -- END TCPServerSimplexFrame::exec()

    bool TCPServerSimplexFrame::quit() 
    {
        std::cout << "BEGIN " << node_alias << "TCPServerSimplexFrame::quit()";
        sync.execute_receive.store(false);
        sync.need_quit.store(true);
        while(!sync.exec_quit_success.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        tcp_server_simplex::base::TCPServerSimplexBase::quit();
        std::cout << "END " << node_alias << " TCPServerSimplexFrame::quit() --- Success" << std::endl;;
        return true;
    } // -- END TCPServerSimplexFrame::quit()

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_FrameSettings &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << sectionName << "::server::frame read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }

        int rc_frame_w_tmp = reader.GetInteger(sectionName, "receive_frame_w", -1);
        if(rc_frame_w_tmp == -1)
        {
            std::cout << "INIReader parse [" << sectionName << "] : receive_frame_w error" << std::endl; 
            return false;
        }
        else
        {
            settings.receive_frame_size.width = rc_frame_w_tmp;
        }
        
        int rc_frame_h_tmp = reader.GetInteger(sectionName, "receive_frame_h", -1);
        if(rc_frame_h_tmp == -1)
        {
            std::cout << "INIReader parse [" << sectionName << "] : receive_frame_h error" << std::endl;
        }
        else
        {
            settings.receive_frame_size.height = rc_frame_h_tmp;
        }

        std::cout << "\t receive_frame_size = [" << rc_frame_w_tmp << ", " << rc_frame_h_tmp << "]" << std::endl;



        std::cout << "END " << sectionName << " read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    void checkValidIniSettings(const RXTX_FrameSettings &settings)
    {
        // throw ...
    } // -- END checkValidIniSettings

    std::shared_ptr<TCPServerSimplexFrame> create(
        const std::string &path2ini, 
        const std::string &sectionName)
    {
        RXTX_FrameSettings rxtx_settings;
        tcp_server_simplex::base::SettingsBase base_settings;

        //чтение настроек базового (родительского) класса
        if(!tcp_server_simplex::base::getIniSettings(path2ini, sectionName, base_settings))
        {
            std::cout << "Error tcp_server_simplex::frame::create(...) -- getIniSettings base_settings error!" << std::endl;
        }
        tcp_server_simplex::base::checkValidIniSettings(base_settings);

        // чтение настроек дочернего класса
        if(!getIniSettings(path2ini, sectionName, rxtx_settings))
        {
            std::cout << "Error tcp_server_simplex::frame::create(...) -- getIniSettings rxtx_settings error!" << std::endl;
        }
        checkValidIniSettings(rxtx_settings);


        auto simplex_server = std::make_shared<TCPServerSimplexFrame>(rxtx_settings, base_settings);
        return simplex_server;
    } // -- END create
    
    bool TCPServerSimplexFrame::wait_receive_frame(cv::Mat &received_frame) const
    {
        auto tp_begin = std::chrono::high_resolution_clock::now();
        int bytes = 0;
        for(int i = 0; i < rc_frame_data_row_len; i+=bytes)
        {
            if((bytes = recv(connfd, frame_buffer + i, rc_frame_data_row_len - i, 0)) == -1) {
                throw std::runtime_error("recv failed");
            }
        }

        //assign pixel values to img
        int ptr = 0;
        for(int i = 0; i < received_frame.rows; ++i)
        {
            for(int j = 0; j < received_frame.cols; ++j)
            {
                received_frame.at<uint8_t>(i,j) = frame_buffer[ptr];
                ptr += 1;
            }
        }

        auto tp_end = std::chrono::high_resolution_clock::now();
        std::cout << "total receive time [ms] = " << 
            std::chrono::duration_cast<std::chrono::microseconds>(tp_end-tp_begin).count() << std::endl;
        return true;
    } // -- END TCPServerSimplex::wait_receive_frame 
} // -- namespace tcp_server_simplex::frame