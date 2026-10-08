#include "cmd400_keeper.hpp"

#include "ExtendINIReader.hpp"

namespace cmd400_keeper
{

bool readIniSettings(const std::string &path2ini, const std::string section, Settings &settings_dst)
{
    // std::string deviceName = "HVGS_Gray_USB_CameraNL";
    std::cout << "BEGIN " << section << "::readIniSettings\n";
    INIReader reader(path2ini);
    if(reader.ParseError() < 0)
    {
        std::cout << "ini reader parse error!\n";
        return false;
    } // END if(reader.ParseError() < 0)

    settings_dst.section_name = section;
    settings_dst.path2ini = path2ini;

    bool ini_report = true;

    std::cout << "END " << section << "::readIniSettings" << std::endl;
    return ini_report;
} // -- END readIniSettings

std::shared_ptr<Cmd400Keeper> create(
    const std::string &path2ini, const std::string &section)
{
    Settings settings;
    if(!readIniSettings(path2ini, section, settings))
    {
        std::cout << "ERROR: call readIniSettings return false" << std::endl;
        return nullptr;
    } // END if(!readIniSettings(path2ini, section, settings))

    std::shared_ptr<Cmd400Keeper> cam_ptr = std::make_shared<Cmd400Keeper>(settings);
    return cam_ptr;
} // -- END create

} // -- namespace cmd400_keeper
