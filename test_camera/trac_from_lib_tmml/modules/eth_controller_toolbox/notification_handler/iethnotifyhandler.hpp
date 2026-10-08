#ifndef IETHNOTIFYHANDLER_HPP
#define IETHNOTIFYHANDLER_HPP

#include <functional>
#include <memory>
#include "eth_notification_common.hpp"

class IEthNotifyHandler
{
public:
	IEthNotifyHandler();
	virtual ~IEthNotifyHandler() {}
	void register_notify_handler(
		const std::function<void(eth_notify_utils::EthNotifyType notify_type, const std::string &src_obj_alias)> &handler);
	virtual void handle(eth_notify_utils::EthNotifyType notify_type, const std::string &src_obj_alias);
private:
	std::function<void(eth_notify_utils::EthNotifyType notify_type, const std::string &src_obj_alias)> handler; 
}; // -- class IEthNotifyHandler

#endif //IETHNOTIFYHANDLER_HPP
