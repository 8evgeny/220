#ifndef ETH_CMD_CODEC_ID_HPP
#define ETH_CMD_CODEC_ID_HPP

#include <cstdint>
#include <memory>

#include "../cmd_id_subcodec_zoo/cmd_id_subcodec_base.hpp"
#include "../cmd_id_common.hpp"

class EthCmdIDCodec // encode and decode id
{
public:
    EthCmdIDCodec(CmdIdCodecTypes codec_type, int pass_decode_id, bool &ok);

    bool check_support(int net_id) const;

    /**
     * Кодирование идентификатора команды в байт-представление.
     * Метод принимает id как целочисленный аргумент и
     * байт-буфер. 
     * @param id - кодируемый (вшиваемый) целочисленный идентификатор
     * @param cmd_bytebuffer - байт-буфер, в который будет закодирован id
     * @return отчет об успешности операции: true-успех, false-ошибка
    */
    bool encode(int id, uint8_t* cmd_bytebuffer) const; 

    /**
     * Декодирование (парсинг) идентификатора команды из байт-представления в 
     * целочисленное. 
     * @param cmd_bytebuffer - байт-буфер, из которого декодируется id
     * @param id - распознанный идентификатор (int)
     * @return отчет об успешности операции: true-успех, false-ошибка
    */
    bool decode(uint8_t const * cmd_bytebuffer, int &id) const; 

    /**
     * Возвращает байтовую длину закодированного id команды 
     * (подразумевается, что эта длина одинакова для всех команд)
    */
    int get_serialize_size() const;
private:
    std::shared_ptr<cmd_id::CmdIdSubcodecBase> codec_core = nullptr;
}; // -- class EthCmdIDCodec

#endif //ETH_CMD_CODEC_ID_HPP
