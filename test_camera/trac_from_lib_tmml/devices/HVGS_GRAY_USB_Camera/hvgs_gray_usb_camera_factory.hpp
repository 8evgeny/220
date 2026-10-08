#ifndef HVGS_GRAY_USB_CAMERA_FACTORY_HPP
#define HVGS_GRAY_USB_CAMERA_FACTORY_HPP

#include "hvgs_gray_usb_camera.hpp"

#include <memory>
#include <string>

namespace devices::hvgs_gray_usb_camera
{
    std::shared_ptr<HVGS_Gray_USB_Camera> create(
        const std::string &path2ini, const std::string &section);
} // -- namespace devices::hvgs_gray_usb_camera

#endif // HVGS_GRAY_USB_CAMERA_FACTORY_HPP