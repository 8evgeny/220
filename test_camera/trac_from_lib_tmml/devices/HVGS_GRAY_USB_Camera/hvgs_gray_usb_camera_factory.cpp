#include "hvgs_gray_usb_camera_factory.hpp"

#include <iostream>
#include "ExtendINIReader.hpp"

namespace devices::hvgs_gray_usb_camera
{

bool readIniSettings(const std::string &path2ini, const std::string section, Settings &settings_dst)
{
    // std::string deviceName = "HVGS_Gray_USB_Camera";
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
    int mirror_hor, mirror_ver;

    ini_report &= ext_ini::GetInteger(reader, section, "camera_id",         settings_dst.camera.id);
    ini_report &= ext_ini::GetInteger(reader, section, "fps",               settings_dst.camera.fps);
    ini_report &= ext_ini::GetInteger(reader, section, "mirror_horizontal", mirror_hor);
    ini_report &= ext_ini::GetInteger(reader, section, "mirror_horizontal", mirror_ver);

    settings_dst.preprocess.mirror_horizontal = static_cast<bool>(mirror_hor);
    settings_dst.preprocess.mirror_vertical = static_cast<bool>(mirror_ver);
    
    std::cout << "END " << section << "::readIniSettings" << std::endl;
    return ini_report;
} // -- END readIniSettings

std::shared_ptr<HVGS_Gray_USB_Camera> create(
    const std::string &path2ini, const std::string &section)
{
    Settings settings;
    if(!readIniSettings(path2ini, section, settings))
    {
        std::cout << "ERROR: call readIniSettings return false" << std::endl;
        return nullptr;
    }
    bool report;
    std::shared_ptr<HVGS_Gray_USB_Camera> cam_ptr = std::make_shared<HVGS_Gray_USB_Camera>(settings, report);
    if(!report)
    {
        return nullptr;
    }
    return cam_ptr;
} // -- END create

} // -- devices::hvgs_gray_usb_camera