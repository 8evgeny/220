#include "cmd_vertelka_keeper_factory.hpp"

#include "ExtendINIReader.hpp"

namespace cmd_vertelka
{

bool readIniSettings(const std::string &path2ini, const std::string section, Settings &settings_dst)
{
    std::cout << "BEGIN " << section << "::readIniSettings\n";
    INIReader reader(path2ini);
    if(reader.ParseError() < 0)
    {
        std::cout << "ini reader parse error!\n";
        return false;
    }

    settings_dst.section_name = section;
    settings_dst.path2ini = path2ini;

    bool ini_report = true;

//    ini_report &= ext_ini::GetInteger(reader, section, "max_delay_btw_packs_ms",    settings_dst.max_delay_btw_packs_ms);

    std::cout << "END " << section << "::readIniSettings" << std::endl;
    return ini_report;
} // -- END readIniSettings

std::shared_ptr<CmdVertelkaKeeper> create(
    const std::string &path2ini, const std::string &section)
{
    Settings settings;
    if(!readIniSettings(path2ini, section, settings))
    {
        std::cout << "ERROR: call readIniSettings return false" << std::endl;
        return nullptr;
    }

    std::shared_ptr<CmdVertelkaKeeper> cam_ptr = std::make_shared<CmdVertelkaKeeper>(settings);
    return cam_ptr;
} // -- END create

} // -- namespace cmd_vertelka_keeper
