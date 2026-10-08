#ifndef CONFIRMATION_CMD_RECEIVE_HPP
#define CONFIRMATION_CMD_RECEIVE_HPP

#include "../eth_cmd_base.hpp"
#include "../../eth_cmd_common_types.hpp"
#include <vector>

/*
    Команда, которая отправляется как уведомление о получении какой-либо команды.
    По содержанию минимально отправляется только шифр (id) поступившей команды.
    TODO: отправлять некоторый ключ верификации/опознавания команды. Например,
    сервером на клиент могут быть отправлены последовательно три команды о 
    о предупреждении трех alarm-событий. По стечению обстоятельств клиент получил 2 из 3 
    команд. Чтобы отличать команды им должна быть назначена метка/токен. Клиент отправляет
    на сервер настоящую команду с подтверждением получения 2х команд. Сервер определяет, что
    поступило два подтверждения из 3х и по метке/шифру определяет, какая команда на дошла до
    адресата.
*/

namespace eth_cmd_processing
{
    class EthCmdConfirmCmdReceive : public EthCmdBase
    {
        public:
            EthCmdConfirmCmdReceive();
            ~EthCmdConfirmCmdReceive();
            virtual int get_serialize_size() const override;
            virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const;
            virtual bool deserialize(uint8_t const * cmd_buffer) override;

            // токен - определенный идентификатор конкретной отправленной команды 
            bool set_data(EthCommands _receive_cmd_id, const std::vector<uint8_t> &_token); 
            void get_data(EthCommands &_receive_cmd_id, std::vector<uint8_t> &_token) const;
        private:
            int serialize_size_bytes = -1;
            EthCommands receive_cmd_id = ETH_CMD_NO_COMMAND; // id команды, на которую поступает ответ
            std::vector<uint8_t> token;
    }; // -- class EthCmdConfirmCmdReceive
} // -- namespace eth_cmd_processing


#endif //CONFIRMATION_CMD_RECEIVE_HPP
