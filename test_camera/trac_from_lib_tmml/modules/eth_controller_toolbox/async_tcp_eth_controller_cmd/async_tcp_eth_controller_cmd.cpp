#include "async_tcp_eth_controller_cmd.hpp"
#include <thread>
#include <chrono>
#include <iostream>

namespace eth_controller::cmd::async
{
    AsyncTcpEthControllerCmd::AsyncTcpEthControllerCmd(
        const AsEthCntSettingsCmd &_settings, 
        std::shared_ptr<eth_controller::cmd::async::client_simplex::AsyncTcpClientSimplexCmd> client_smp_ptr,
        std::shared_ptr<eth_controller::cmd::async::server_simplex::AsyncTcpServerSimplexCmd> server_smp_ptr) :
            settings(_settings), _client_simplex(client_smp_ptr), _server_simplex(server_smp_ptr)
    {
        
    } // constructor AsyncTcpEthControllerCmd

    AsyncTcpEthControllerCmd::~AsyncTcpEthControllerCmd()
    {

    } // -- END ~AsyncTcpEthControllerCmd

    void AsyncTcpEthControllerCmd::exec_client_simplex()
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

    void AsyncTcpEthControllerCmd::exec_server_simplex()
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

    bool AsyncTcpEthControllerCmd::launch()
    {
#ifdef _WIN32
        WSADATA WSData;
        int i_res = WSAStartup(MAKEWORD(2, 2), &WSData); // 2.2 version
        if(i_res != 0)
        {
            std::cout << "ERROR: failed call WSAStartup(...) in AsyncUDPEthControllerCmd" << std::endl;
            return false;
        }
#endif //_WIN32

        std::thread _server_simplex_workflow_thread(
            &eth_controller::cmd::async::AsyncTcpEthControllerCmd::exec_server_simplex, this);
        _server_simplex_workflow_thread.detach();

        std::thread _client_simplex_workflow_thread(
            &eth_controller::cmd::async::AsyncTcpEthControllerCmd::exec_client_simplex, this);
        _client_simplex_workflow_thread.detach();
        
        while(! (_client_simplex->is_connect_established() && _server_simplex->is_connect_established() ) )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_established_duplex_connection_delay_ms));
        }
        
        sync._execute_rx_tx.store(true);

        return true;
    } // -- END launch()

    bool AsyncTcpEthControllerCmd::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        return _client_simplex->send_cmd(cmd);
    } // -- END send_cmd

    void AsyncTcpEthControllerCmd::register_cmd_handler(ICmdHandler *handler)
    {
        _server_simplex->register_cmd_handler(handler);
    } // -- END register_cmd_handler

    void AsyncTcpEthControllerCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
        this->_client_simplex->register_notify_handler(handler);
        this->_server_simplex->register_notify_handler(handler);
    } // -- END register_notify_handler

    bool AsyncTcpEthControllerCmd::quit()
    {
        std::cout << "BEGIN AsyncTcpEthControllerCmd::_client_simplex->quit()" << std::endl;
        _client_simplex->quit();
        std::cout << "END AsyncTcpEthControllerCmd::_client_simplex->quit()" << std::endl;

        std::cout << "BEGIN AsyncTcpEthControllerCmd::_server_simplex->quit()" << std::endl;
        _server_simplex->quit();
        std::cout << "END AsyncTcpEthControllerCmd::_server_simplex->quit()" << std::endl;

        int watchdog_total_loop_try = 20;
        int attempt_counter = 0;
        bool wait_status = true;
        while(! (_client_simplex->is_success_quit() && _server_simplex->is_success_quit()))
        {
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
            std::cout << "SUCCESS close AsyncTcpEthControllerCmd" << std::endl;
        }
        else
        {
            std::cout << "ERROR close AsyncTcpEthControllerCmd" << std::endl;
        }
        // _client_simplex->close_socket(true);
        // _server_simplex->close_socket(true);
#ifdef _WIN32
        WSACleanup();
#endif
        return wait_status;
    } // -- END quit
} // namespace eth_controller::cmd::async

