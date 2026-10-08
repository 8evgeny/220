#include "tcp_server_simplex_cmd.hpp"

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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

#include "../../command_processing/eth_cmd_classification.hpp"


namespace chrono = std::chrono;

namespace tcp_server_simplex::cmd
{
    TCPServerSimplexCmd::TCPServerSimplexCmd(
        RXTX_CmdSettings rxtx_settings_, 
        tcp_server_simplex::base::SettingsBase base_settings_) : rxtx_settings(rxtx_settings_)
    {
        this->settings_base = base_settings_;
        cmd_bytebuffer = new uint8_t[rxtx_settings.single_command_buf_size_byte]; // предварительное выделение буфера с запасом по объему

        eth_cmd_demux = eth_cmd_processing::EthCmdDemux();
    } // -- END TCPServerSimplexCmd

    TCPServerSimplexCmd::TCPServerSimplexCmd()
    {

    } // -- END TCPServerSimplexCmd

    TCPServerSimplexCmd::~TCPServerSimplexCmd()
    {
        close(sockfd);
        close(connfd);
        this->TCPServerSimplexBase::~TCPServerSimplexBase();
    } // -- END ~TCPServerSimplexCmd

    bool TCPServerSimplexCmd::exec()
    {
        std::cout << "BEGIN " << node_alias << " TCPServerSimplexCmd::exec()" << std::endl;
        std::cout << "sync.execute_receive.load() = " << sync.execute_receive.load() << std::endl;
        std::shared_ptr<eth_cmd_processing::EthCmdBase> receive_cmd;
        while(sync.execute_receive.load())
        {
            int n_bytes_read = read(connfd, cmd_bytebuffer, rxtx_settings.single_command_buf_size_byte);
            if(n_bytes_read == 0)
            {
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
                    std::cout << "attempt inroccrect cmd, skip" << std::endl;
                    continue;
                }
                // уведомление подписчиков о вновь поступившей валидной команде
                for(auto handler : cmd_handlers)
                {
                    handler->handle(receive_cmd);
                }
            }

            // if(!is_connected(connfd))
            // {
            //     break;
            // }
            //std::cout << counter++ << std::endl;
            std::this_thread::sleep_for(std::chrono::microseconds(150)); // оптимизационный момент, чтобы слишком часто не принимать cmd
            //wait_receive_cmd({1000,700}, 1, test_mat);
        } // -- END while(sync.execute_receive.load())
        std::cout << node_alias << " -- SimplexServer::disconnection done" << std::endl;
        std::cout << node_alias << "TCPServerSimplexFrame socket success closed" << std::endl;
        sync.exec_quit_success.store(true);
        std::cout << "END " << node_alias << " TCPServerSimplexCmd::exec()" << std::endl;
        return true;
    } // -- END exec

    bool TCPServerSimplexCmd::quit()
    {
        std::cout << "BEGIN TCPServerSimplexCmd::quit()" << std::endl;

        close(sockfd);
        close(connfd);

        sync.execute_receive.store(false);
        sync.need_quit.store(true);
        while(!sync.exec_quit_success.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::cout << "END TCPServerSimplexCmd::quit() --- Success" << std::endl;
        return true;
    } // -- END quit

    void TCPServerSimplexCmd::register_cmd_handler(ICmdHandler *handler)
    {
        cmd_handlers.push_back(handler);
    } // -- END register_cmd_handler

    void TCPServerSimplexCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        notify_handlers.push_back(handler);
    } // -- END register_notify_handler

    void TCPServerSimplexCmd::apply_notify_all_handlers(eth_notify_utils::EthNotifyType notify_type)
    {
        std::string composite_alias = settings_base.net_settings.network_node_alias + "::ServerSimplex";
        for(auto handler : notify_handlers)
        {
            handler->handle(notify_type, composite_alias);
        }
    } // -- END apply_notify_all_handlers

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_CmdSettings &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << sectionName << "::server_simplex::cmd read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            throw std::runtime_error("INIReader ParseError ini file path=\"" + path2ini + "\"");
        }

        //pass

        std::cout << "END " << sectionName << "::server_simplex::cmd read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(
        const RXTX_CmdSettings &settings)
    {
        //pass
        return true;
    }

    std::shared_ptr<TCPServerSimplexCmd> create(
        const std::string &path2ini, 
        const std::string &sectionName)
    {
        RXTX_CmdSettings rxtx_settings;
        tcp_server_simplex::base::SettingsBase base_settings;

        std::cout << "BEGIN create " << sectionName << " TCPServerSimplexCmd" << std::endl;

        if(!tcp_server_simplex::base::getIniSettings(path2ini, sectionName, base_settings))
        {
            std::cout << "Error tcp_server_simplex::cmd::create(...) -- getIniSettings base_settings error!" << std::endl;
        }
        if(!tcp_server_simplex::base::checkValidIniSettings(base_settings))
        {
            std::cout << "Error tcp_server_simplex::cmd::create(...) -- checkValidIniSettings base_settings error!" << std::endl;
        }

        if(!getIniSettings(path2ini, sectionName, rxtx_settings))
        {
            std::cout << "Error tcp_server_simplex::cmd::create(...) -- getIniSettings rxtx_settings error!" << std::endl;
        }
        if(!checkValidIniSettings(rxtx_settings))
        {
            std::cout << "Error tcp_server_simplex::cmd::create(...) -- checkValidIniSettings rxtx_settings error!" << std::endl;
        }

        auto server = std::make_shared<TCPServerSimplexCmd>(rxtx_settings, base_settings);
        std::cout << "END create " << sectionName << " TCPServerSimplexCmd" << std::endl;

        return server;
    } // -- END create

} // -- namespace tcp_server_simplex::base