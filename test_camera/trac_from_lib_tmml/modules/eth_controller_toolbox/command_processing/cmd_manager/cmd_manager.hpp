#ifndef ETH_CMD_MANAGER_HPP
#define ETH_CMD_MANAGER_HPP

#include <memory>

#include "command_processing/eth_cmd_common_types.hpp"
#include "command_processing/cmd_preambule_tools/cmd_id/cmd_id_codec/eth_cmd_id_codec.hpp"
#include "command_processing/cmd_tail_tools/checksum/checksum_codec/checksum_codec.hpp"
#include "command_processing/commands_zoo/eth_cmd_factory.hpp"
#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "command_processing/cmd_tail_tools/checksum/checksum_common.hpp"
#include "command_processing/cmd_tail_tools/checksum/checksum_tools/checksum_tools.hpp" // TMP

#include "command_processing/table_cmd_id_replace/table_cmd_id_replace.hpp"
#include "eth_cntrl_common_types.hpp"

namespace eth_cmd_manager
{

struct Settings
{
    Settings() { }
    Settings(
        CmdIdCodecTypes     _cmd_id_codec_type, 
        EthCommands         _pass_decode_cmd_type, 
        ChecksumCodecTypes  _checksum_codec_type) :
            cmd_id_codec_type(_cmd_id_codec_type),
            pass_decode_cmd_type(_pass_decode_cmd_type),
            checksum_codec_type(_checksum_codec_type) 
    {
        
    }

    /* ### HEAD ### */
    CmdIdCodecTypes cmd_id_codec_type               = CMD_ID_CODEC_SIMPLE_DIRECT_ENC;
    EthCommands pass_decode_cmd_type                = ETH_CMD_AP_TELEMETRY; // при CMD_ID_CODEC_PASS
    
    // bool use_default_gateway_cmd_type = false;  // ОПАСНАЯ ФУНКЦИЯ! Использовать в случае полной уверенности, что нераспознанные по head preamble id будут далее обрабатываться как default_gateway_cmd_type
    // EthCommands default_gateway_cmd_type    = ETH_CMD_AP_TELEMETRY;

    TableCmdIdReplace tir_net2host;
    TableCmdIdReplace tir_host2net;

    /* ### TAIL ###*/
    ChecksumCodecTypes checksum_codec_type          = CHECKSUM_CODEC_PASS;

    // ChecksumCodecSettings checksum_codec;
    // IdCodecSettings id_codec;
};

class CmdManager
{
public:
    CmdManager(const Settings &_settings, bool &ok);

    /**
     * Переназначение id команды из системы координат Eth_Controller_Toolbox
     * в систему координат ПИВ (протокол информационного взаимодействия).
     * Переназначение на отправку и прием (в общем случае разное).
    */
    bool set_replace_cmd_id(
        EthCommands                         host_id,    // id команды на host'е
        int                                 net_id,     // id команда в сети
        eth_common::CmdDirection            direction); // направление распространения команды

    /* ### SEND ### */
    bool set_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> _cmd);

    /**
     * Возвращает размер команды в сериализованном виде. 
     * Применяется при предварительном динамическом выделении памяти.
    */
    int get_serialize_size() const; 

    /**
     * Производится запись в предварительно выделенный участок памяти 
     * (performance-ориентировано)
    */
    bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const; 

    /* ### RECEIVE ### */
    bool parse_cmd(uint8_t const * cmd_buffer, std::shared_ptr<eth_cmd_processing::EthCmdBase> &command) const;

private:
    CmdManager() = delete;
    bool replace_cmd_id(EthCommands host_id, int net_id, TableCmdIdReplace &_tir);
    Settings settings;
    std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd = nullptr;
    std::shared_ptr<EthCmdIDCodec> idCodec = nullptr;
    std::shared_ptr<EthCmdChecksumCodec> checksumCodec = nullptr;
    eth_cmd_processing::EthCmdFactory factory;
}; // -- class CmdManager

} // -- namespace cmd_manager

#endif //ETH_CMD_MANAGER_HPP
