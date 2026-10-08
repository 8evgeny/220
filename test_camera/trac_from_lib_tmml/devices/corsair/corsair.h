#ifndef CORSAIR_H
#define CORSAIR_H

#include "devices/device.hpp"
#include "eth/corsair_ethcontroller.h"
#include "cmd/corsair_cmdmanager.h"
#include "tools/INIReader.h"

class Corsair : public Device
{
public:
    // конструктор с установкой имени сетевой карты
    Corsair(std::string path_to_settings, bool &ok)
    {
        ok = false;
        if(getSettings(path_to_settings)){
            _ethController = &Corsair_EthController::inst();
            _CmdManager = new Corsair_CmdManager();
            ok = true;
        }
        else
            delete this;
    }

    void workflow() override;
    void keyHandler(char &key) override;

private:
    bool getSettings(const std::string path_to_ini) override;

    void select_tvgray_chanel() override;
    void select_tvrgb_chanel() override;
    void zoom_in() override;
    void zoom_out() override;
    void focus_near() override;
    void focus_far() override;


    bool zoom_started = false;
    bool focus_started = false;
    uint32_t _currChanel = CORSAIR_CHANNEL_ID_TEST;
};

#endif // CORSAIR_H
