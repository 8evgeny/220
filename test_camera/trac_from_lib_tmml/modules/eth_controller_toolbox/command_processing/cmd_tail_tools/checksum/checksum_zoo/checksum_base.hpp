#ifndef CHECKSUM_BASE_HPP
#define CHECKSUM_BASE_HPP

#include <cstdint>
#include <vector>
#include <iostream>

namespace checksum
{

class ChecksumBase
{
public:
    ChecksumBase(){}
    virtual ~ChecksumBase() {}
    virtual int get_serialize_size() const
    {
        return serialize_size;
    }
    virtual bool calculate(uint8_t const * arr, int len, std::vector<uint8_t> &checksum_dst) const
    {
        std::cout << "ERROR: not call ChecksumBase::calculate() not valid behavioral!" << std::endl;
        return false;
    }
protected:
    int serialize_size = -1;
}; // -- class ChecksumBase

} // -- namespace checksum

#endif //CHECKSUM_BASE