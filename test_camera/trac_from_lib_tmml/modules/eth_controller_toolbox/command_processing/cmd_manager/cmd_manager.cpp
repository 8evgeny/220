#include "cmd_manager.hpp"

#include <iostream>

namespace eth_cmd_manager
{

CmdManager::CmdManager(const Settings &_settings, bool &ok) : settings(_settings)
{
    bool ok_id = false, ok_cs = false;
    idCodec = std::make_shared<EthCmdIDCodec>(
        settings.cmd_id_codec_type, 
        settings.pass_decode_cmd_type,
        ok_id);
    checksumCodec = std::make_shared<EthCmdChecksumCodec>(
        settings.checksum_codec_type,
        ok_cs);
    ok = ok_id & ok_cs;
} // -- END CmdManager(const Settings &_settings)

bool CmdManager::set_replace_cmd_id(
    EthCommands     host_id,
    int                                 net_id,
    eth_common::CmdDirection            direction)
{
    if(direction == eth_common::CMD_DIRECTION_HOST_2_NET)
    {
        return replace_cmd_id(host_id, net_id, settings.tir_host2net);
    }
    else if(direction == eth_common::CMD_DIRECTION_NET_2_HOST)
    {
        return replace_cmd_id(host_id, net_id, settings.tir_net2host);
    }
    return false;
} // -- END set_replace_cmd_id

bool CmdManager::replace_cmd_id(EthCommands host_id, int net_id, TableCmdIdReplace &_tir)
{
    // check-in use idCodec 
    /*
        Для установки очередного правила перевода host2net и net2host
        требуется определить, поддерживает ли кодек net_id,
        который передан аргументом в настоящую функцию.
        Например, при "простейшем" правиле кодирования используется 1 байт uchar,
        возможный диапазон целых чисел 0...255
    */

    if(!idCodec->check_support(net_id))
    {
        return false;
    }
    if(!_tir.set(host_id, net_id))
    {
        return false;
    }
    return true;
} // -- END set_replace_id

bool CmdManager::set_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> _cmd)
{
    if(_cmd == nullptr)
    {
        return false;
    }
    cmd = _cmd;
    return true;
} // -- END set_cmd

int CmdManager::get_serialize_size() const 
{
    int bytesize = 0;
    bytesize += idCodec->get_serialize_size();
    bytesize += cmd->get_serialize_size();
    bytesize += checksumCodec->get_serialize_size();
    return bytesize;
} // -- END get_serialize_size

bool CmdManager::serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const
{
    int cmd_sum_size = 0;
    uint8_t* cmd_buf_org_ptr = cmd_buffer;

    /* #### ENCODE HEAD #### */
    int cmd_net_id;
    if(!settings.tir_host2net.get_host2net(cmd->get_id(), cmd_net_id))
    {
        return false;
    }
    if(!idCodec->encode(cmd_net_id, cmd_buffer))
    {
        return false;
    }
    int prb_id_size = idCodec->get_serialize_size(); // preamble id size
    cmd_sum_size += prb_id_size;
    cmd_buffer += prb_id_size;

    /* #### ENCODE CORE CMD CONTENT #### */
    int core_size = 0;
    if(!cmd->serialize_noalloc(cmd_buffer, core_size))
    {
        return false;
    }
    cmd_sum_size += core_size;
    cmd_buffer += core_size; //TODO: сделать корректно через передачу указателя в cmd->serialize по ссылке


    /* #### ENCODE TAIL #### */
    int tail_checksum_size = checksumCodec->get_serialize_size();
    if(!checksumCodec->encode(cmd_buf_org_ptr, cmd_buffer))
    {
        return false;
    }

    cmd_buffer += tail_checksum_size; // if last, then not need inc
    cmd_sum_size += tail_checksum_size;

    buffer_size_bytes = cmd_sum_size;
    return true;
} // -- END serialize_noalloc

bool CmdManager::parse_cmd(uint8_t const * cmd_buffer, std::shared_ptr<eth_cmd_processing::EthCmdBase> &command) const
{
    int cmd_sum_size = 0;
    uint8_t const * cmd_buf_org_ptr = cmd_buffer;

    /* #### DECODE HEAD #### */
    int cmd_net_id = 0;
    if(!idCodec->decode(cmd_buffer, cmd_net_id))
    {
        return false;
    }
    EthCommands cmd_host_id = ETH_CMD_NO_COMMAND;
    if(!settings.tir_net2host.get_net2host(cmd_net_id, cmd_host_id))
    {
        return false;
    }
    int prb_id_size = idCodec->get_serialize_size(); // preamble id size
    cmd_sum_size += prb_id_size;
    cmd_buffer += prb_id_size;

    /* #### ENCODE CORE CMD CONTENT #### */
    std::shared_ptr<eth_cmd_processing::EthCmdBase> _cmd = factory.create(cmd_host_id);

    if (_cmd == nullptr)
    {
        return false;
    }
    if(!_cmd->deserialize(cmd_buffer))
    {
        return false;
    }
    int cmd_core_size = _cmd->get_serialize_size();
    cmd_buffer += cmd_core_size;
    cmd_sum_size += cmd_core_size;

    /* #### ENCODE TAIL #### */
    if(!checksumCodec->check(cmd_buf_org_ptr, cmd_buffer))
    {
        std::cout << "ERROR: checksum check failed" << std::endl;
        return false;
    }
    int checksum_size = checksumCodec->get_serialize_size();
    cmd_buffer += checksum_size;
    cmd_sum_size += checksum_size;
    
    command = _cmd;
    return true;
} // -- END parse_cmd

} // -- namespace cmd_manager