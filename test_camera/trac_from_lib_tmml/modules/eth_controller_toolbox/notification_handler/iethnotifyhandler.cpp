#include "iethnotifyhandler.hpp"

IEthNotifyHandler::IEthNotifyHandler()
{

}

void IEthNotifyHandler::register_notify_handler(
    const std::function<void(eth_notify_utils::EthNotifyType notify_type, const std::string &src_obj_alias)> &handler)
{
    this->handler = handler;
}

void IEthNotifyHandler::handle(eth_notify_utils::EthNotifyType notify_type, const std::string &src_obj_alias)
{
    handler(notify_type, src_obj_alias);
}