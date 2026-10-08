#ifndef IMX477_SQUARE_CAMERA_MIPI_FACTORY_HPP
#define IMX477_SQUARE_CAMERA_MIPI_FACTORY_HPP

#include <memory>
#include <string>
#include "imx477_square_camera_mipi.hpp"

namespace devices::imx477_square_camera_mipi
{
    UserSettings readSettings(const std::string &pathToSettings, bool &success);
    bool checkValidAndAdaptationSettings(UserSettings &settings);
    std::shared_ptr<IMX477SquareCamMIPI> create(const std::string &config_path, int & dev_fps);
} // -- END namespace devices::imx219_camera_mipi

#endif // IMX477_SQUARE_CAMERA_MIPI_FACTORY_HPP
