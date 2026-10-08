#include "copter_alarm_eth_cmd.hpp"
#include "../../eth_cmd_common_types.hpp"
#include <cstring>
#include <cstdint>
#include <cassert>
#include "../../eth_serialization_tools.hpp"

/*
    Поле №0(служебное), Тип поля - uint8_t, Длина - 1 (байт), Описание - 
    Поле №1, Тип поля - int, Длина - 4 (байт), Описание - Пиксельная координата "X" коптера
    Поле №2, Тип поля - int, Длина - 4 (байт), Описание - Пиксельная координата "Y" коптера
    Поле №3, Тип поля - int, Длина - 4 (байт), описание - Пиксальная площадь коптера
*/

namespace eth_cmd_processing
{
    EthCmdCopterAlarm::EthCmdCopterAlarm()
    {
        serialize_size_bytes = sizeof(EthCmdDataCopterAlarm) + 1; // bytes (+1 -- 1 байт резервируется под id команды)
        cmd_id = ETH_CMD_COPTER_ALARM;
    } // -- END EthCmdCopterAlarm::EthCmdCopterAlarm()
    
    int EthCmdCopterAlarm::get_serialize_size() const
    {
        return serialize_size_bytes;
    } // -- END get_serialize_size()

    bool EthCmdCopterAlarm::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
    {
        *cmd_buffer = cmd_id; cmd_buffer++; // назначается id команды TODO: повысить безопасность используя номер команды закодированный и зашифрованный
        int s0 = 1;
        int s1 = eth_serialization::little_endian::conv2byteArrPushBack_int32t(cmd_data.target_x_pos_pix, cmd_buffer);
        int s2 = eth_serialization::little_endian::conv2byteArrPushBack_int32t(cmd_data.target_y_pos_pix, cmd_buffer);
        int s3 = eth_serialization::little_endian::conv2byteArrPushBack_int32t(cmd_data.target_area_pix, cmd_buffer);
        assert(s0 + s1 + s2 + s3 == serialize_size_bytes);
        buffer_size_bytes = serialize_size_bytes;
        return true;
    } // -- END serialize_noalloc()

    bool EthCmdCopterAlarm::deserialize(uint8_t const *cmd_buffer)
    {
        cmd_buffer++; //id команды - пропускаем байт 
        cmd_data.target_x_pos_pix = eth_deserialization::little_endian::parse_int32(cmd_buffer);
        cmd_data.target_y_pos_pix = eth_deserialization::little_endian::parse_int32(cmd_buffer);
        cmd_data.target_area_pix = eth_deserialization::little_endian::parse_int32(cmd_buffer); 
        return true;
    } // -- END deserialize()

    void EthCmdCopterAlarm::set_target_data(int32_t x_pos, int32_t y_pos, int32_t area)
    {
        this->cmd_data.target_x_pos_pix = x_pos;
        this->cmd_data.target_y_pos_pix = y_pos;
        this->cmd_data.target_area_pix = area;
    } // -- END set_target_data

    EthCmdDataCopterAlarm EthCmdCopterAlarm::get_cmd_data()
    {
        return cmd_data;
    }
};