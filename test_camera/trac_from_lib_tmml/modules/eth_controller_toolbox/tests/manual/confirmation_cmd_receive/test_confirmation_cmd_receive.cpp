#include <iostream>
#include "command_processing/commands_zoo/confirmation_cmd_receive/confirmation_cmd_receive.hpp"
#include "command_processing/commands_zoo/eth_cmd_factory.hpp"
#include "command_processing/eth_cmd_classification.hpp"
#include <memory>
#include <cstdint>
#include <iostream> 
#include <ostream>

template <typename T> 
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) 
{ 
    os << "[";
    for (int i = 0; i < v.size(); ++i) { 
        os << (int)v[i]; 
        if (i != v.size() - 1) 
            os << ", "; 
    }
    os << "]";
    return os; 
}

int main(int argc, char* argv[])
{
    uint8_t* cmd_buf = new uint8_t[100];
    std::vector<uint8_t> token_src = {1,2,4,8,16,32,64,128};
    EthCommands cmd_id_src = ETH_CMD_POWEROFF_REQUEST;

    std::cout << "cmd_id_src = " << cmd_id_src << std::endl;
    std::cout << "token_src = " << token_src << std::endl;

    std::shared_ptr<eth_cmd_processing::EthCmdConfirmCmdReceive> cmd_conf_rec;
    cmd_conf_rec = std::make_shared<eth_cmd_processing::EthCmdConfirmCmdReceive>();
    cmd_conf_rec->set_data(cmd_id_src, token_src);
    int cmd_len_src;
    cmd_conf_rec->serialize_noalloc(cmd_buf, cmd_len_src);

    eth_cmd_processing::EthCmdDemux demux_obj = eth_cmd_processing::EthCmdDemux();

    std::shared_ptr<eth_cmd_processing::EthCmdBase> eth_cmd_dst_base;
    demux_obj.eth_cmd_demux_to_handler(cmd_buf, eth_cmd_dst_base);

    eth_cmd_dst_base->deserialize(cmd_buf);
    
    int cmd_len_dst = eth_cmd_dst_base->get_serialize_size();

    std::cout << "Bytes: [";
    for(int i = 0; i < cmd_len_dst; ++i)
    {
        std::cout << (int)cmd_buf[i] << " ";
    }
    std::cout << "]" << std::endl;

    std::shared_ptr<eth_cmd_processing::EthCmdConfirmCmdReceive> cmd_conf_rec_dst;

    cmd_conf_rec_dst = std::dynamic_pointer_cast<eth_cmd_processing::EthCmdConfirmCmdReceive>(eth_cmd_dst_base);

    EthCommands cmd_id_dst;
    std::vector<uint8_t> token_dst;
    cmd_conf_rec_dst->get_data(cmd_id_dst, token_dst);

    std::cout << "cmd_id_dst = " << cmd_id_dst << std::endl;
    std::cout << "token_dst = " << token_dst << std::endl; 


return 0;
}
