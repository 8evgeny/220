#ifndef ETH_CMD_BASE_HPP
#define ETH_CMD_BASE_HPP

#include <cstdint>

#include "command_processing/eth_cmd_common_types.hpp"

namespace eth_cmd_processing
{
    class EthCmdBase
    {
        public:
            EthCmdBase();
            EthCommands get_id() const;

            virtual int get_serialize_size() const; // возвращает размер команды в сериализованном виде. Применяется при предварительном динамическом выделении памяти.
            
            /*
                Сериализация транслирует команду в массив байтов по установленным правилам протокола;
                -- buffer возвращает указатель на начало данных в памяти устройства;
                -- buffer_size_bytes - возвращает длину буфера команды в байтах
            */
            virtual bool serialize_alloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const; // выделяется динамическая память
            virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const; // производится запись в предварительно выделенный участок памяти (perfomance-ориентировано)

            /*
                Десериализация разворачивает (декодирует) буффер байт-команду, формирует ее поля в классе (отложенный "конструктор")
            */
            virtual bool deserialize(uint8_t const * cmd_buffer);
        protected:
            EthCommands cmd_id; // Уникальный идентификатор типа команды; 
    }; // -- END class EthCmdBase
}

#endif // ETH_CMD_BASE_HPP
