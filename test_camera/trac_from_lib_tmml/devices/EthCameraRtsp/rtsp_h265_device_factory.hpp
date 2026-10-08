#ifndef RTSP_H265_DEVICE_FACTORY_HPP
#define RTSP_H265_DEVICE_FACTORY_HPP

#include <memory>
#include <string>
#include "rtsp_h265_device.hpp"
#include "tools/INIReader.h"
#include "tools/ExtendINIReader.hpp"
#include "tools/toolbox_common_utils/toolbox_common_utils.hpp"

namespace devices::rtsp_h265_device
{
    SettingsPtr readSettings(const std::string &pathToSettings, const std::string &ini_section_name, bool &success);
    bool checkValidAndAdaptationSettings(SettingsPtr settings);
    std::shared_ptr<RTSP_H265_device> create(const std::string &config_path, const std::string &ini_section_name, int& dev_fps);

} // -- END namespace devices::imx219_camera_mipi

#endif // RTSP_H265_DEVICE_FACTORY_HPP
