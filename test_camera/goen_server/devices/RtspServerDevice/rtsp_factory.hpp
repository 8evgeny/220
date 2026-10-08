#ifndef RTSP_FACTORY_HPP
#define RTSP_FACTORY_HPP
#include <memory>
#include <string>
#include "rtsp.hpp"
#include "tools/INIReader.h"
#include "tools/ExtendINIReader.hpp"
#include "tools/toolbox_common_utils/toolbox_common_utils.hpp"

namespace devices::rtsp
{
    SettingsPtr readSettings(const std::string &pathToSettings, const std::string &ini_section_name, bool &success);
    bool checkValidAndAdaptationSettings(SettingsPtr settings);
    std::shared_ptr<RTSP> create(const std::string &config_path, const std::string &ini_section_name);
} // -- END namespace devices::rtsp
#endif // END #ifndef RTSP_FACTORY_HPP
