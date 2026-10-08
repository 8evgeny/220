#include "device.hpp"

void Device::setup()
{
#ifdef USE_ETHERNET_CONNECTION
    // попытка открытия соккетов клиента и сервера
    if(!_ethController->start(dev,
                              src_mac, src_ip, src_port,
                              dst_mac, dst_ip, dst_port))
    {
        std::cout << "Can't open socket" << std::endl;
        _ethController->stop();
        return;
    } // -- END if(!_ethController->start(dev, src_mac, src_ip, src_port, dst_mac, dst_ip, dst_port))
    std::cout << "Sockets opened successfully" << std::endl;
#endif
}

#ifdef USE_ETHERNET_CONNECTION
CmdManager *Device::getCmdManager()
{
    return _CmdManager;
}

void Device::register_command_handler(ICommandHandler *handler)
{
    Server* srv = _ethController->getServer();
    srv->getHandlers()->push_back(handler);
}
#endif

uint8_t *Device::receiveFrame(int &w, int &h, int &id, int &num)
{
#ifdef USE_ETHERNET_CONNECTION
    uint8_t *img_data = _ethController->rcv_frame(w, h, id, num);
    return img_data;
#else
    return nullptr;
#endif
}

bool Device::sendFrame(const uint8_t *f, int w, int h, int id, int num)
{
#ifdef USE_ETHERNET_CONNECTION
    std::cout << "Send frame: " << w << "x" << h << std::endl;
    return _ethController->send_frame(f, w, h, id, num);
#else
    return false;
#endif
}
