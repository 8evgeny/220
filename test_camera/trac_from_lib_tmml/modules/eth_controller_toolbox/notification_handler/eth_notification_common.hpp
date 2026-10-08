#ifndef NOTIFICATION_COMMON_HPP
#define NOTIFICATION_COMMON_HPP

namespace eth_notify_utils
{
    enum EthNotifyType
    {   
        ETH_CONNECT_BROKEN_PEER_DISCONNECT, // ethernet соединение разорвано
        ETH_CONNECT_BROKEN_ERROR_READ, // ошибка чтения
        ETH_CONNECT_ERROR_SEND_CMD // ошибка отправки eth-команды
    }; // -- enum NotifyType
} // -- namespace notify_utils

#endif //NOTIFICATION_COMMON_HPP
