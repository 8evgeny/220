#include "eth_serialization_tools.hpp"
#include <cstring>
#include <stdio.h>
#include <algorithm>

void ReverseBytes( uint8_t *start, int size )
{
    uint8_t *istart = start, *iend = istart + size;
    std::reverse(istart, iend);
} // -- END ReverseBytes

namespace eth_serialization::little_endian
{
    int conv2byteArrPushBack_int32t(int32_t data, uint8_t* &byte_arr_ptr) 
    {
        *byte_arr_ptr = (data >> 0) & 0xFF; byte_arr_ptr++; 
        *byte_arr_ptr = (data >> 8) & 0xFF; byte_arr_ptr++;
        *byte_arr_ptr = (data >> 16) & 0xFF; byte_arr_ptr++;
        *byte_arr_ptr = (data >> 24) & 0xFF; byte_arr_ptr++; // наиболее значащий байт кодируется последним
        return 4;
    } // -- END void conv2byteArrPushBack_int32t(int32_t data, uint8_t* byte_arr_ptr)

    int conv2byteArrPushBack_float32(float data, uint8_t* &byte_arr_ptr)
    {
        const int float_size = 4;
        memcpy(byte_arr_ptr, reinterpret_cast<uint8_t*>(&data), float_size);

        #ifdef SYSTEM_BIG_ENDIAN
        ReverseBytes(byte_arr_ptr, float_size);
        #endif //SYSTEM_BIG_ENDIAN

        byte_arr_ptr += float_size;
        return 4;
    } // -- END conv2byteArrPushBack_float

    int conv2byteArrPushBack_double64(double data, uint8_t* &byte_arr_ptr)
    {
        const int double_size = 8;
        memcpy(byte_arr_ptr, reinterpret_cast<uint8_t*>(&data), double_size);
        
        #ifdef SYSTEM_BIG_ENDIAN
        ReverseBytes(byte_arr_ptr, double_size);
        #endif //SYSTEM_BIG_ENDIAN
        
        byte_arr_ptr += double_size;
        return 8;
    } // -- END conv2byteArrPushBack_double
    
    int conv2byteArrPushBack_string(const std::string &data, uint8_t* &byte_arr_ptr)
    {
        char const * data_ptr = data.data();
        int int32_size = conv2byteArrPushBack_int32t(data.size(), byte_arr_ptr);
        std::memcpy(byte_arr_ptr, data_ptr, data.size()); // нет необходимости специализировать endian-style
        byte_arr_ptr += data.size();
        return int32_size + data.size();
    } // -- END conv2byteArrPushBack_string
} // -- END namespace eth_serialization

namespace eth_deserialization::little_endian
{
    int32_t parse_int32(uint8_t const * &byte_arr_ptr)
    {
        int32_t value;
        value = (byte_arr_ptr[3] << 24) | (byte_arr_ptr[2] << 16) | (byte_arr_ptr[1] << 8) | (byte_arr_ptr[0]);
        byte_arr_ptr += 4;
        return value;
    } // -- END parse_int32

    float parse_float32(uint8_t const * &byte_arr_ptr)
    {
        #ifdef SYSTEM_BIG_ENDIAN
        ReverseBytes(byte_arr_ptr, 4);
        #endif //SYSTEM_BIG_ENDIAN

        float value = *(reinterpret_cast<float const *>(byte_arr_ptr));
        byte_arr_ptr += 4;
        return value;
    } // -- END parse_float32

    double parse_double64(uint8_t const * &byte_arr_ptr)
    {
        #ifdef SYSTEM_BIG_ENDIAN
        ReverseBytes(byte_arr_ptr, 8);
        #endif //SYSTEM_BIG_ENDIAN
        
        double value = *(reinterpret_cast<double const *>(byte_arr_ptr));
        byte_arr_ptr += 8;
        return value;
    } // -- END parse_double

    std::string parse_string(uint8_t const * &byte_arr_ptr)
    {
        int str_size = parse_int32(byte_arr_ptr);
        char const * const str_symbol_ptr = reinterpret_cast<char const *>(byte_arr_ptr);
        std::string parsed_string = std::string(str_symbol_ptr, str_size);
        byte_arr_ptr += str_size;
        return parsed_string;
    } // -- END parse_string
} // -- END namespace eth_deserialization
