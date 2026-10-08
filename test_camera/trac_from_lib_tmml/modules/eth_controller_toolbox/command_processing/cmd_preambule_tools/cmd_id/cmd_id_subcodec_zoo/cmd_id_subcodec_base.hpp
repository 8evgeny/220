#ifndef CMD_ID_SUBCODEC_BASE_HPP
#define CMD_ID_SUBCODEC_BASE_HPP

#include <cstdint>
#include <iostream>

namespace cmd_id
{

class CmdIdSubcodecBase
{
public:
    CmdIdSubcodecBase() {}
    virtual ~CmdIdSubcodecBase() {}
    virtual bool check_support(int net_id) const
    {
        log_error_call("check_support");
        return false;
    }
    virtual int get_serialize_size() const
    {
        return serialize_size;
    }
    virtual bool encode(int id, uint8_t* cmd_bytebuffer) const
    {
        log_error_call("encode");
        return false;
    }
    virtual bool decode(uint8_t const * cmd_bytebuffer, int &id) const
    {
        log_error_call("decode");
        return false;
    }
protected:
    int serialize_size = - 1;
private:
    void log_error_call(const std::string &_msg) const
    {
        std::cout << "ERROR: not valid call CmdIdSubcodecBase::" << _msg << " ---> not valid behavioral!" << std::endl;
    }
}; // -- class CmdIdSubcodecBase

} // -- namespace cmd_id

#endif //CMD_ID_SUBCODEC_BASE_HPP
