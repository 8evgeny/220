#ifndef COLIBRITV_H
#define COLIBRITV_H

#include <string>
#include <iostream>

#include "devices/device.hpp"
#include "eth/colibritv_ethcontroller.h"
#include "tools/INIReader.h"

class ColibriTV : public Device
{
public:
    // конструктор с установкой имени сетевой карты
    ColibriTV(const std::string &path_to_settings, bool &ok);

    void workflow();
    void keyHandler(char &key);

    int getColorChannels() override;
    bool isBayerColorChannel() override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;

    void register_frame_handler(IFrameHandler *handler);

    void quit() override;
private:
    std::string output_format = "GRAY";
    bool getSettings(const std::string &path_to_ini) override;
};

#endif // COLIBRITV_H
