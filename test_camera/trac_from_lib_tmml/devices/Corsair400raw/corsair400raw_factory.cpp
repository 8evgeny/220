#include "corsair400raw_factory.hpp"

#include <iostream>
#include "ExtendINIReader.hpp"

namespace devices::corsair_400_raw
{

bool readIniSettings(const std::string &path2ini, const std::string section, Settings &settings_dst)
{
    // std::string deviceName = "Corsair400raw";
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
    int mirror_hor, mirror_ver;

    ini_report &= ext_ini::Get(reader,          section, "rcv_ip",   settings_dst.net.rcv_ip);
    ini_report &= ext_ini::GetInteger(reader,   section, "rcv_port",         settings_dst.net.rcv_port);

    
    std::cout << "END " << section << "::readIniSettings" << std::endl;
    return ini_report;
} // -- END readIniSettings

std::shared_ptr<Corsair400raw> create(
    const std::string &path2ini, const std::string &section)
{
    Settings settings;
    if(!readIniSettings(path2ini, section, settings))
    {
        std::cout << "ERROR: call readIniSettings return false" << std::endl;
        return nullptr;
    } // END if(!readIniSettings(path2ini, section, settings))
    bool report;
    std::shared_ptr<Corsair400raw> cam_ptr = std::make_shared<Corsair400raw>(settings, report);
    if(!report)
    {
        return nullptr;
    } // END if(!report)
    return cam_ptr;
} // -- END create

} // -- devices::corsair_400_raw
