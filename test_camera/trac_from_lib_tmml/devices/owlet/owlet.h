#ifndef OWLET_H
#define OWLET_H

#include <fstream>
#include <iostream>
#include "devices/device.hpp"
#include "eth/owlet_ethcontroller.h"
#include "cmd/owlet_cmdmanager.h"
#include "tools/INIReader.h"

class Owlet : public Device
{
public:
    // конструктор с установкой имени сетевой карты
    Owlet(std::string path_to_settings, bool &ok)
    {
        ok = false;
        if(getSettings(path_to_settings)){
            _ethController = &Owlet_EthController::inst();
            _CmdManager = new Owlet_CmdManager();
            ok = true;
        }
        else
            delete this;
    }

    void workflow() override;
    void keyHandler(char &key) override;
    void updateAimPosition(std::pair<float, float> pos) override;

private:
    struct speed{
        int vert = 0;
        int hor = 0;
    };

    std::string way2log = "../log.txt";
    std::string way2calib = "../calib.txt";
    std::pair<float, float> cent_obj_rel_default = {0.5, 0.5};
    std::pair<float, float> cent_obj_rel = cent_obj_rel_default;
    std::pair<float, float> object_relative;
    int speed_manual = 2000; // Параметр ручной скорости дрейфа совенка.
    int speed_auto = 1000; // Параметр автоматической скорости дрейфа совенка.
    speed sp = speed{0, 0};

    void select_tvgray_chanel() override;
    void select_tvrgb_chanel() override;


    bool getSettings(const std::string path_to_ini) override;
    bool FileIsExist(const std::string& filePath);

    bool zoom_started = false;
    bool focus_started = false;
    uint32_t _currChanel = 0;
};
#endif // OWLET_H
