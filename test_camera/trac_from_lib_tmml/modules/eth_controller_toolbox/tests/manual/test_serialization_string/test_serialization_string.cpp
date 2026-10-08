#include <iostream>

#include "command_processing/eth_serialization_tools.hpp"
#include <cstdint>
#include <string>

int main(int argc, char* argv[])
{
    uint8_t* byte_buf = new uint8_t[100];
    std::string src_str = "abcd1234efgh5678_";
    int pack_len = eth_serialization::little_endian::conv2byteArrPushBack_string(src_str, byte_buf);

    std::cout << "src str = " << src_str << std::endl;

    std::cout << "Byte array: ";
    for(int i = 0; i <pack_len; ++i)
    {
        std::cout << (int)byte_buf[i] << " ";
    }
    std::cout << "\n";

    std::string dst_str;
    
    uint8_t const * buf = byte_buf;
    dst_str = eth_deserialization::little_endian::parse_string(buf);
    
    std::cout << "dst str = " << dst_str << std::endl;

return 0;
}
