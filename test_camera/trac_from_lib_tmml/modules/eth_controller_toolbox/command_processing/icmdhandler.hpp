#ifndef ICMDHANDLER_HPP
#define ICMDHANDLER_HPP


#include <functional>
#include <memory>
#include "commands_zoo/eth_cmd_base.hpp"

class ICmdHandler
{
public:
    ICmdHandler();
    virtual ~ICmdHandler() {}
    void register_cmd_handler(
            const std::function<void(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)> &handler);
    virtual void handle(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd);

private:
    std::function<void(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)> handler;
};

#endif //ICMDHANDLER_HPP
