#ifndef GST_PIPELINE_DEVICE_FACTORY_HPP
#define GST_PIPELINE_DEVICE_FACTORY_HPP

#include <memory>
#include <string>
#include "gst_pipeline_device.hpp"
#include "tools/INIReader.h"
//#include "tools/ExtendINIReader.hpp"
//#include "tools/toolbox_common_utils/toolbox_common_utils.hpp"

namespace devices::gst_pipeline_device
{
    std::string readSettings(const std::string &pathToSettings, const std::string &ini_section_name, bool &success);
    std::shared_ptr<devices::GST_PIPELINE_device> create(const std::string &config_path, bool & ok,  const std::string &ini_section_name, const std::string & location = "location");

} // -- END namespace devices::imx219_camera_mipi

#endif // GST_PIPELINE_DEVICE_FACTORY_HPP
