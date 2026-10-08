#include "icmdhandler.hpp"

ICmdHandler::ICmdHandler()
{

}

void ICmdHandler::register_cmd_handler(
        const std::function<void(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)> &handler)
{
    this->handler = handler;
}

void ICmdHandler::handle(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
{
    handler(cmd);
}
