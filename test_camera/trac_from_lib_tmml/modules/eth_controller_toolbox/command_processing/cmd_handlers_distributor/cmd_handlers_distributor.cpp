#include "cmd_handlers_distributor.hpp"
#include <iostream>

namespace eth_cmd_processing
{
    CmdHandlersDistributor::CmdHandlersDistributor()
    {

    } // -- END CmdHandlersDistributor 

    bool CmdHandlersDistributor::apply(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        if(cmd == nullptr)
        {
            std::cout << "Error CmdHandlersDistributor::apply(...) cmd==nullptr!" << std::endl;
            return false;
        }

        EthCommands cmd_id = static_cast<EthCommands>(cmd->get_id());
        if(cmd_id == ETH_CMD_NO_COMMAND)
        {
            std::cout << "Error: incorrect cmd -- is ptr base class" << std::endl;
        } // END if(cmd_id == ETH_CMD_NO_COMMAND)

        if(handlers_data.find(cmd_id) == handlers_data.end())
        {
            std::cout << "Warning: not found registered handlers for process cmd. cmd_id = " << cmd_id << std::endl; //TODO: сделать общедоступный инструмент конвертации enum EthCommands cmd -> string 
            return false;
        } // END if(handlers_data.find(cmd_id) == handlers_data.end())
        else
        {
            for(int i = 0; i < handlers_data[cmd_id].size(); ++i)
            {
                handlers_data[cmd_id][i](cmd);
            } // END for(int i = 0; i < handlers_data[cmd_id].size(); ++i)
        } // END else
        return true;
    } // -- END apply

    void CmdHandlersDistributor::register_cmd_handler(EthCommands cmd_id, 
        const std::function<void(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)> &handler)
    {
        handlers_data[cmd_id].push_back(handler);
    } // -- end register_cmd_handler

} // namespace eth_cmd_processing
