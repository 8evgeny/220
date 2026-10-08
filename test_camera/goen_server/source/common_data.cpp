#include "common_data.hpp"

common_data::common_data(){}
common_data::~common_data(){}

bool common_data::is_need_quit()
{
    return need_quit;
}//END is_need_quit()

void common_data::set_need_quit(bool new_need_quit)
{
    need_quit = new_need_quit;
}//END set_need_quit(bool new_need_quit)

bool common_data::is_error_rs232()
{
    return error_rs232;
}//END get_error_rs232()

void common_data::set_error_rs232(bool new_error_rs232)
{
    error_rs232 = new_error_rs232;
}//END set_error_rs232(bool newError_rs232)

bool common_data::is_error_rs485()
{
    return error_rs485;
}//END get_error_rs485()

void common_data::set_error_rs485(bool new_error_rs485)
{
    error_rs485 = new_error_rs485;
}//END set_error_rs485(bool newError_rs485)

bool common_data::is_init_all_ports()
{
    return init_all_ports;
}//END is_init_all_ports()
void common_data::set_init_all_ports(bool new_init_all_ports)
{
    init_all_ports = new_init_all_ports;
}//END set_init_all_ports(bool new_init_all_ports)

