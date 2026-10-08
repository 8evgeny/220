#ifndef ETH_CNTRL_COMMON_TYPES_HPP
#define ETH_CNTRL_COMMON_TYPES_HPP

#include <cstdint>

namespace eth_common
{
    /*
        При настройке EthController'а есть возможность специализировать модуль 
        по назначению. В SIMPLEX режиме ресурсоемкость меньше, чем в DUPLEX; 
    */
    enum CommunicationModes : uint8_t // режим eth-взаимодействия EthController'а;
    {
        SIMPLEX_TRANSMIT, // односторонний режим коммуникации только на отправку
        SIMPLEX_RECEIVE, // односторонний режим коммуникации только на прием 
        DUPLEX, // прием+отправка 
        NOT_DEFINE // назначается в случае технической ошибки парсинга режима
    }; // -- END CommunicationModes

    /*
    Для классификации команды по типу требуется ответ / не требуется вводится параметр команды из CommandAnswerRequestTypes
    */
    enum CommandAnswerRequestTypes : uint8_t
    {
        NOTIFICATION, // команда является уведомлением, без запроса на ответ
        ANSWER, // команда является ответом на полученный запрос
        REQUEST, // команда является запросом
    }; // -- END CommandAnswerRequestTypes

    /*
    Используется для определения конкретного типа при полиморфной работе с указателем на базовый класс для upcast'а
    */
    enum EthControllerType : uint8_t
    {
        FRAME,
        CMD
    }; // -- END EthControllerType 
    
    /*
    Направление команды (из программы (host) в сеть (net) или наоборот)
    */ 
    enum CmdDirection
    {
        CMD_DIRECTION_HOST_2_NET,
        CMD_DIRECTION_NET_2_HOST
    };

} // eth_common

#endif //ETH_CNTRL_COMMON_TYPES_HPP