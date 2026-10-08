#ifndef TABLE_CMD_ID_REPLACE
#define TABLE_CMD_ID_REPLACE

#include "command_processing/eth_cmd_common_types.hpp"
#include <vector>
#include <unordered_map>

/* Таблица переназначения ID своя у источника и приемника (client /  server)*/
/* 
    Используется для назначения внешних коммутационных ID, 
    отличных от внутрипрограммных. Внутрипрограммные идентификаторы
    команд назначены в соответствии с правилом порядковой нумерации элементов
    типа перечисления (enum), но при внешней коммутации может возникнуть 
    задача переназначить ID, при этом для корректного перевода ID 
    существует таблица перевода ID - TableCmdIdReplace (TID);
*/
class TableCmdIdReplace
{
public:
    TableCmdIdReplace();
    bool set(EthCommands cmd_type, int cmd_id);
    bool get_net2host(int cmd_id, EthCommands &cmd_type) const; // при получении по eth
    bool get_host2net(EthCommands cmd_type, int &cmd_id) const; // при отправке по eth
    void log_content() const;
private:
    std::unordered_map<int, EthCommands> net2enum;
    std::unordered_map<EthCommands, int> enum2net;
}; // -- class TableCmdIdReplace

#endif // TABLE_CMD_ID_REPLACE