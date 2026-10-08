#include "owlet.h"

void Owlet::workflow()
{
    _CmdManager->set_speed(sp.vert, sp.hor);
    sp = speed{0, 0};
    //   cout  << "sp.hor=" << sp.hor << "; sp.vert=" << sp.vert << endl;
}

void Owlet::keyHandler(char &key)
{
    if(key)
    {
        switch(key)
        {
        case 'q':
        {
            sp = speed{0, 0};
            //cout << " q ( stop )\n";
        }
            break;
        case '4':
        {
            sp = speed{0, -speed_manual};
            //cout << " 4 ( left )\n";
        }
            break;
        case '6':
        {
            sp = speed{0, speed_manual};
            //cout << " 6 ( right )\n";
        }
            break;
        case '8':
        {
            sp = speed{-speed_manual, 0};
            //cout << " 8 ( up )\n";
        }
            break;
        case '2':
        {
            sp = speed{speed_manual, 0};
            //cout << " 2 ( down )\n";
        }
            break;
        case '7':
        {
            sp = speed{-speed_manual, -speed_manual};
            //cout << " 7 ( left-up )\n";
        }
            break;
        case '9':
        {
            sp = speed{-speed_manual, speed_manual};
            //cout << " 9 ( right-up )\n";
        }
            break;
        case '1':
        {
            sp = speed{speed_manual, -speed_manual};
            //cout << " 1 ( left-down )\n";
        }
            break;
        case '3':
        {
            sp = speed{speed_manual, speed_manual};
            //cout << " 3 ( right-down )\n";
        }
            break;
            //         case '5':
            //         {
            //             sp = speed{0, 0};
            //             //cout << " 5 ( stop )\n";
            //         }
            //         break;
        case 'c':
        {
            cent_obj_rel.first = cent_obj_rel_default.first - object_relative.first;
            cent_obj_rel.second = cent_obj_rel_default.second - object_relative.second;
            if(FileIsExist(way2calib))
            {
                std::string del_log = "rm " + way2calib;
                system(del_log.c_str());
            } // -- END if(FileIsExist(way2calib))
            std::cout << " Calibrate\n";
        }
            break;
        case 'd':
        {
            cent_obj_rel = cent_obj_rel_default;
            std::cout << " Decalibrate\n";
        }
            break;
            //         case 27:
            //         {
            //             _cmdManager->set_speed(0, 0);
            //             cout << "ESC -> Exit" << endl;
            //             stop();
            //             return 0;
            //         }
            //         break;
        } // -- END switch(sr->key)
    } // -- END if(sr->key)
}

void Owlet::updateAimPosition(std::pair<float, float> pos)
{
    sp = speed{0, 0};
    object_relative.first = pos.first - cent_obj_rel.first;
    object_relative.second = pos.second - cent_obj_rel.second;
    int speed_vert = int(speed_auto*object_relative.second);
    int speed_hor = int(speed_auto*object_relative.first);
    sp = speed{speed_vert, speed_hor};
}

void Owlet::select_tvgray_chanel()
{
    _CmdManager->select_channel(OWLET_CHANNEL_ID_TVGRAY);
    _currChanel = OWLET_CHANNEL_ID_TVGRAY;
}

void Owlet::select_tvrgb_chanel()
{
    _CmdManager->select_channel(OWLET_CHANNEL_ID_TVRGB);
    _currChanel = OWLET_CHANNEL_ID_TVRGB;
}

bool Owlet::getSettings(const std::string path_to_ini)
{
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){
        std::cout << "ini reader: Parse error\n";
        return false;
    }

    std::string temp_dev = reader.Get("Owlet", "lan_card", "oops");
    if(temp_dev == "oops"){
        std::cout << "ini reader: Parse lan_card error\n";
        return false;
    }
    else
        dev = temp_dev;

    std::string temp_src_mac = reader.Get("Owlet", "src_mac", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_mac error\n";
        return false;
    }
    else
        src_mac = temp_src_mac;

    std::string temp_src_ip = reader.Get("Owlet", "src_ip", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_ip error\n";
        return false;
    }
    else
        src_ip = temp_src_ip;

    int temp_src_port = reader.GetInteger("Owlet", "src_port", -1);
    if(temp_src_port == -1){
        std::cout << "ini reader: Parse src_port error\n";
        return false;
    }
    else
        src_port = temp_src_port;

    std::string temp_dst_ip = reader.Get("Owlet", "dst_ip", "oops");
    if(temp_dst_ip == "oops"){
        std::cout << "ini reader: Parse dst_ip error\n";
        return false;
    }
    else
        dst_ip = temp_dst_ip;

    std::string temp_dst_mac = reader.Get("Owlet", "dst_ip", "oops");
    if(temp_dst_mac == "oops"){
        std::cout << "ini reader: Parse dst_mac error\n";
        return false;
    }
    else
        dst_mac = temp_dst_mac;

    int temp_dst_port = reader.GetInteger("Owlet", "dst_port", -1);
    if(temp_dst_port == -1){
        std::cout << "ini reader: Parse dst_port error\n";
        return false;
    }
    else
        dst_port = temp_dst_port;


    return true;
}

bool Owlet::FileIsExist(const std::string &filePath)
{
    bool isExist = false;
    std::ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
}
