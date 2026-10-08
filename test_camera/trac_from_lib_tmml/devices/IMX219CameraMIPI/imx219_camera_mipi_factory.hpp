#ifndef IMX219_CAMERA_MIPI_FACTORY_HPP
#define IMX219_CAMERA_MIPI_FACTORY_HPP

#include <memory>
#include <string>
#include "imx219_camera_mipi.hpp"

namespace devices::imx219_camera_mipi
{
    Settings readSettings(const std::string &pathToSettings);
    bool checkValidAndAdaptationSettings(Settings &settings);
    std::shared_ptr<IMX219CameraMIPI> create(const std::string &config_path);

} // -- END namespace devices::imx219_camera_mipi

#endif // IMX219_CAMERA_MIPI_FACTORY_HPP