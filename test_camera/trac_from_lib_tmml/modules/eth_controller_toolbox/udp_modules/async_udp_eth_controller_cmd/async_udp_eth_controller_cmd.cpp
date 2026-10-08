#include "async_udp_eth_controller_cmd.hpp"

#include <thread>
#include <chrono>
#include <iostream>

namespace eth_controller::cmd::async::udp
{
    AsyncUDPEthControllerCmd::AsyncUDPEthControllerCmd(
        const AsyncUDPEthControllerCmdSettings &_settings, 
        std::shared_ptr<eth_controller::cmd::async::udp::client_simplex::AsyncUDPClientSimplexCmd> client_smp_ptr,
        std::shared_ptr<eth_controller::cmd::async::udp::server_simplex::AsyncUDPServerSimplexCmd> server_smp_ptr) :
            settings(_settings), _client_simplex(client_smp_ptr), _server_simplex(server_smp_ptr)
    {
        support_tx = settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_TRANSMIT ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX;
        support_rx = settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_RECEIVE ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX;
    } // constructor AsyncUDPEthControllerCmd

    AsyncUDPEthControllerCmd::~AsyncUDPEthControllerCmd()
    {
        std::cout << "Деструктор AsyncUDPEthControllerCmd" << std::endl;
        quit();
    } // -- END ~AsyncUDPEthControllerCmd

    bool AsyncUDPEthControllerCmd::set_minor_settings(const MinorSettings &_minor_settings)
    {
        if(sync.launch_was_call.load())
        {
            return false;
        }
        minor_settings = _minor_settings;

        if(!update_minor_settings())
        {
            std::cout << "ERROR: update_minor_settings failed" << std::endl;
        }
        return true;
    } // -- END set_minor_settings

    MinorSettings AsyncUDPEthControllerCmd::get_minor_settings()
    {
        return minor_settings;
    } // -- END get_minor_settings

    bool AsyncUDPEthControllerCmd::update_minor_settings()
    {
        if(sync.launch_was_call.load())
        {
            return false;
        }

        if(support_rx)
        {
            server_simplex::MinorSettings srv_mn_settings;
            srv_mn_settings.cmdManager = minor_settings.cmd_manager_server;
            if(!_server_simplex->set_minor_settings(srv_mn_settings))
            {
                return false;
            }
        }

        if(support_tx)
        {
            client_simplex::MinorSettings cli_mn_settings;
            cli_mn_settings.cmdManager = minor_settings.cmd_manager_client;
            if(!_client_simplex->set_minor_settings(cli_mn_settings))
            {
                return false;
            }
        }  
        return true;
    } // -- END update_minor_settings

    void AsyncUDPEthControllerCmd::exec_client_simplex()
    {
        if(!_client_simplex->setup())
        {
            std::cout << "Error setup _client_simplex" << std::endl;
        }

        while(!sync._execute_rx_tx.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_established_duplex_connection_delay_ms));
        }

        _client_simplex->exec();
    } // -- END exec_client_simplex

    void AsyncUDPEthControllerCmd::exec_server_simplex()
    {
        if(!_server_simplex->setup())
        {
            std::cout << "Error setup _server_simplex" << std::endl;
        }

        while(!sync._execute_rx_tx.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_established_duplex_connection_delay_ms));
        }

        _server_simplex->exec();
    } // -- END exec_server_simplex

    bool AsyncUDPEthControllerCmd::launch()
    {
        if(sync.launch_was_call.load())
        {
            return sync._execute_rx_tx.load();
        }
        update_minor_settings();

        sync.launch_was_call.store(true);

#ifdef _WIN32
        WSADATA WSData;
        int i_res = WSAStartup(MAKEWORD(2, 2), &WSData); // 2.2 version
        if(i_res != 0)
        {
            std::cout << "ERROR: failed call WSAStartup(...) in AsyncUDPEthControllerCmd" << std::endl;
            return false;
        }
#endif //_WIN32

        if(support_rx)
        {
            std::thread _server_simplex_workflow_thread(
                &eth_controller::cmd::async::udp::AsyncUDPEthControllerCmd::exec_server_simplex, this);
            _server_simplex_workflow_thread.detach();
        }

        if(support_tx)
        {
            std::thread _client_simplex_workflow_thread(
                &eth_controller::cmd::async::udp::AsyncUDPEthControllerCmd::exec_client_simplex, this);
            _client_simplex_workflow_thread.detach();
        }
        
        switch(settings.communication_mode)
        {
            case eth_common::SIMPLEX_RECEIVE:
            {
                while(! _server_simplex->is_connect_established() )
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(wait_established_duplex_connection_delay_ms));
                }
            }
            break;

            case eth_common::SIMPLEX_TRANSMIT:
            {
                while(! _client_simplex->is_connect_established() )
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(wait_established_duplex_connection_delay_ms));
                }
            }
            break;

            case eth_common::DUPLEX:
            {
                while(! (_client_simplex->is_connect_established() && _server_simplex->is_connect_established() ) )
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(wait_established_duplex_connection_delay_ms));
                }
            }
            break;

            default:
            {
                std::cout <<"ERROR: wrong settings.communication_mode" << std::endl;
                return false;
            }
            break;
        } // -- switch(settings.communication_mode)

        std::cout << "SUCCESS: AsyncUDPEthControllerCmd::launch()" << std::endl;
        sync._execute_rx_tx.store(true);

        return true;
    } // -- END launch()

    bool AsyncUDPEthControllerCmd::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        if(support_tx)
        {
            return _client_simplex->send_cmd(cmd);
        }
        return false;
    } // -- END send_cmd

    void AsyncUDPEthControllerCmd::register_cmd_handler(ICmdHandler *handler)
    {
        if(support_rx)
        {
            _server_simplex->register_cmd_handler(handler);
        }
        else
        {
            std::cout << "ERROR: call AsyncUDPEthControllerCmd::register_cmd_handler, but NOT support rx!" << std::endl;
        }
    } // -- END register_cmd_handler

    void AsyncUDPEthControllerCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
        if(support_tx)
        {
            this->_client_simplex->register_notify_handler(handler);
        }
        if(support_rx)
        {
            this->_server_simplex->register_notify_handler(handler);
        }
    } // -- END register_notify_handler

    bool AsyncUDPEthControllerCmd::quit()
    {
        if(sync.quit_was_called.load())
        {
            // std::cout << settings.node_alias << "::AsyncUDPEthControllerCmd::quit() was called earlier" << std::endl;
            return false;
        }
        sync.quit_was_called.store(true);

        if(support_tx)
        {
            std::cout << "BEGIN AsyncUDPEthControllerCmd::_client_simplex->quit()" << std::endl;
            _client_simplex->quit();
            std::cout << "END AsyncUDPEthControllerCmd::_client_simplex->quit()" << std::endl;
        }

        if(support_rx)
        {
            std::cout << "BEGIN AsyncUDPEthControllerCmd::_server_simplex->quit()" << std::endl;
            _server_simplex->quit();
            std::cout << "END AsyncUDPEthControllerCmd::_server_simplex->quit()" << std::endl;
        }

        int watchdog_total_loop_try = 200;
        int attempt_counter = 0;
        bool wait_status = true;
        bool is_closed = false;
        while(true)
        {
            is_closed = true;
            if(support_tx)
            {
                is_closed &= _client_simplex->is_success_quit();
            }
            if(support_rx)
            {
                is_closed &= _server_simplex->is_success_quit(); 
            }
            if(is_closed)
            {
                wait_status = true;
                break;
            }
            if(attempt_counter >= watchdog_total_loop_try)
            {
                wait_status = false;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++attempt_counter;
        }

        if(wait_status)
        {
            std::cout << "SUCCESS close AsyncUDPEthControllerCmd" << std::endl;
            if(support_rx)
            {
                _server_simplex->free_buffer();
            }
            if(support_tx)
            {
                _client_simplex->free_buffer();
            }
        }
        else
        {
            std::cout << "ERROR close AsyncUDPEthControllerCmd" << std::endl;
        }
#ifdef _WIN32
        WSACleanup();
#endif // _WIN32
        // _client_simplex->close_socket(true);
        // _server_simplex->close_socket(true);
        return wait_status;
    } // -- END quit
} // namespace eth_controller::cmd::async::udp