#include <iostream>

#include "command_processing/table_cmd_id_replace/table_cmd_id_replace.hpp"

int main(int argc, char* argv[])
{
    TableCmdIdReplace tir;
    tir.set(ETH_CMD_AP_TELEMETRY,           1);
    tir.set(ETH_CMD_COPTER_ALARM,           3);
    tir.set(ETH_CMD_CONFIRM_CMD_RECEIVE,    7);

    tir.log_content();

    int ap_tele_cmd_eth_ext_id,
        copt_alarm_cmd_eth_ext_id,
        conf_rcv_cmd_eth_ext_id;

    if(!(
        tir.get_host2net(ETH_CMD_AP_TELEMETRY, ap_tele_cmd_eth_ext_id) &
        tir.get_host2net(ETH_CMD_COPTER_ALARM, copt_alarm_cmd_eth_ext_id) &
        tir.get_host2net(ETH_CMD_CONFIRM_CMD_RECEIVE, conf_rcv_cmd_eth_ext_id)
        ))
    {
        return 1;
    }

    std::cout << "ETH_CMD_AP_TELEMETRY: enum_id = " << (int)ETH_CMD_AP_TELEMETRY << "; ---> net_id = " << ap_tele_cmd_eth_ext_id << std::endl;
    std::cout << "ETH_CMD_COPTER_ALARM: enum_id = " << (int)ETH_CMD_COPTER_ALARM << "; ---> net_id = " << copt_alarm_cmd_eth_ext_id << std::endl;
    std::cout << "ETH_CMD_CONFIRM_CMD_RECEIVE: enum_id = " << (int)ETH_CMD_CONFIRM_CMD_RECEIVE << "; ---> net_id = " << conf_rcv_cmd_eth_ext_id << std::endl;
    return 0;
}
