#ifndef FENIX2META_ETH_CMD_HPP
#define FENIX2META_ETH_CMD_HPP

#include "../eth_cmd_base.hpp"
#include "../fenix2meta/fenix2meta_struct.hpp"

namespace eth_cmd_processing
{

class EthCmdFenix2MetaData : public EthCmdBase
{
public:
    EthCmdFenix2MetaData();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
    void set(const Fenix2Meta &_trackmeta);
    void get(Fenix2Meta &_trackmeta);
private:
    int serialize_size_bytes = -1;
    Fenix2Meta fenix2meta;
}; // -- class EthCmdFenix2MetaData

} // -- namespace eth_cmd_processing

#endif //FENIX2META_ETH_CMD_HPP
