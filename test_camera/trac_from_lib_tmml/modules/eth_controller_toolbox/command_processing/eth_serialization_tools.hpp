#ifndef ETH_SERIALIZATION_TOOLS_HPP
#define ETH_SERIALIZATION_TOOLS_HPP

#include <cstdint>
#include <string>
/*
    Для удобства некоторых cast-операций используется little-endian порядок байтов
    при формировании и интерпретации байт-массива. (Несмотря, что стандартом сетевого взаимодействия является big-endian).
*/

namespace eth_serialization::little_endian
{
    int conv2byteArrPushBack_int32t(int32_t data, uint8_t* &byte_arr_ptr); // возвращает длину записанного участка в байтах
    int conv2byteArrPushBack_float32(float data, uint8_t* &byte_arr_ptr);
    int conv2byteArrPushBack_double64(double data, uint8_t* &byte_arr_ptr);
    int conv2byteArrPushBack_string(const std::string &data, uint8_t* &byte_arr_ptr); // Warning, команда может быть длинной и вызывать segmentation fault
} // -- END namespace eth_serialization

namespace eth_deserialization::little_endian
{
    int32_t parse_int32(uint8_t const * &byte_arr_ptr);
    float parse_float32(uint8_t const * &byte_arr_ptr);
    double parse_double64(uint8_t const * &byte_arr_ptr);
    std::string parse_string(uint8_t const * &byte_arr_ptr);
} // -- END namespace eth_deserialization

#endif //ETH_SERIALIZATION_TOOLS_HPP