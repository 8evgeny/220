#pragma once

class common_data
{
public:
    common_data();
    ~common_data();
    bool is_need_quit();
    void set_need_quit(bool new_need_quit);
    bool is_error_rs232();
    void set_error_rs232(bool new_error_rs232);
    bool is_error_rs485();
    void set_error_rs485(bool new_error_rs485);
    bool is_init_all_ports();
    void set_init_all_ports(bool new_init_all_ports);
    bool receive_CMD_over_UDP = false;

private:
    bool need_quit = false;
    bool error_rs232 = false;
    bool error_rs485 = false;
    bool init_all_ports = false;

};//END class common_data

