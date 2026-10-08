#include "table_cmd_id_replace.hpp"
#include <iostream>

TableCmdIdReplace::TableCmdIdReplace()
{
    net2enum.clear(); // maybe not need clear
    enum2net.clear();
} // TableCmdIdReplace()

bool TableCmdIdReplace::set(EthCommands cmd_type, int cmd_id)
{
    if(enum2net.count(cmd_type) != 0) // если был ранее назначен спец id, то его нужно удалить из контейнера net2enum
    {
        int match_id = enum2net[cmd_type];
        if(net2enum.count(match_id) == 0)
        {
            std::cout << "ERROR: failed match enum2net and net2enum; Wrong use TableCmdIdReplace" << std::endl;
            return false;
        }
        else
        {
            if(net2enum.erase(match_id) == 0)
            {
                std::cout << "ERROR: manage content net2enum by match_id failed" << std::endl;
                return false;
            }
        }
    }
    enum2net[cmd_type] = cmd_id;
    net2enum[cmd_id] = cmd_type;
    return true;
} // -- END set

bool TableCmdIdReplace::get_net2host(int cmd_id, EthCommands &cmd_type) const
{
    if(net2enum.count(cmd_id) == 0)
    {
        if(cmd_id >= 0 && cmd_id < ETH_CMD_LAST_ELEMENT_NO_SHIFT - 1)
        {
            cmd_type =  static_cast<EthCommands>(cmd_id);
        }
        else 
        {
            cmd_type = ETH_CMD_NO_COMMAND;
            return false;
        }
    }
    else
    {
        cmd_type = net2enum.at(cmd_id);
    }
    return true;
} // -- END get 

bool TableCmdIdReplace::get_host2net(EthCommands cmd_type, int &cmd_id) const
{
    if(enum2net.count(cmd_type) == 0)
    {
        cmd_id = static_cast<int>(cmd_type);
    }
    else
    {
        cmd_id = enum2net.at(cmd_type);
    }
    return true;
} // -- END get

void TableCmdIdReplace::log_content() const
{
    std::cout << std::string(10, '*') << " LOG TIR CONTENT " << std::string(10, '*') << std::endl;
    for(auto& it : enum2net)
    {
        std::cout << "\tHOST_ID = " << it.first << " : NET_ID = " << it.second << std::endl;
    }
} // -- END log_content