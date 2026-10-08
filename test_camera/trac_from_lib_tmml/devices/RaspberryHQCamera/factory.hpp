#ifndef RASPBERRYHQCAMERA_FACTORY_HPP
#define RASPBERRYHQCAMERA_FACTORY_HPP

#include <memory>

#include "raspberryhqcamera.hpp"

namespace devices::raspberry_hq_camera {
    std::shared_ptr<RaspberryHQCamera> create(const std::string &pathToSettings);
}

#endif // RASPBERRYHQCAMERA_FACTORY_HPP
