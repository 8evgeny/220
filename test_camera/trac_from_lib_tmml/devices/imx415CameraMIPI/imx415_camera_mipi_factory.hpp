#ifndef IMX415_CAMERA_MIPI_FACTORY_HPP
#define IMX415_CAMERA_MIPI_FACTORY_HPP

#include <memory>
#include <string>
#include "imx415_camera_mipi.hpp"

namespace devices::imx415_camera_mipi
{
    UserSettings readSettings(const std::string &pathToSettings, bool &success);
    bool checkValidAndAdaptationSettings(UserSettings &settings);
    std::shared_ptr<IMX415CamMIPI> create(const std::string &config_path);

} // -- END namespace devices::imx219_camera_mipi

#endif // IMX415_CAMERA_MIPI_FACTORY_HPP
