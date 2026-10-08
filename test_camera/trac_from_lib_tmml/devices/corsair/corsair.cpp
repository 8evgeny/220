#include "corsair.h"

void Corsair::workflow()
{
    if(zoom_started){
        _CmdManager->zoom_stop(_currChanel);
        zoom_started = false;
    }
    if(focus_started){
        _CmdManager->focus_stop(_currChanel);
        focus_started = false;
    }
}

void Corsair::keyHandler(char &key)
{
    switch(key){
    case 'q':{
        if(!zoom_started)
            zoom_out();
    }
        break;
    case 'w':{
        if(!zoom_started)
            zoom_in();
    }
        break;
    case 'a':{
        if(!focus_started)
            focus_far();
        }
        break;
    case 's':{
        if(!focus_started)
            focus_near();
        break;
    }
    default:
        break;
    }
}

bool Corsair::getSettings(const std::string path_to_ini)
{
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){
        std::cout << "ini reader: Parse error\n";
        return false;
    }

    std::string temp_dev = reader.Get("Corsair", "lan_card", "oops");
    if(temp_dev == "oops"){
        std::cout << "ini reader: Parse lan_card error\n";
        return false;
    }
    else
        dev = temp_dev;

    std::string temp_src_mac = reader.Get("Corsair", "src_mac", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_mac error\n";
        return false;
    }
    else
        src_mac = temp_src_mac;

    std::string temp_src_ip = reader.Get("Corsair", "src_ip", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_ip error\n";
        return false;
    }
    else
        src_ip = temp_src_ip;

    int temp_src_port = reader.GetInteger("Corsair", "src_port", -1);
    if(temp_src_port == -1){
        std::cout << "ini reader: Parse src_port error\n";
        return false;
    }
    else
        src_port = temp_src_port;

    std::string temp_dst_ip = reader.Get("Corsair", "dst_ip", "oops");
    if(temp_dst_ip == "oops"){
        std::cout << "ini reader: Parse dst_ip error\n";
        return false;
    }
    else
        dst_ip = temp_dst_ip;

    std::string temp_dst_mac = reader.Get("Corsair", "dst_ip", "oops");
    if(temp_dst_mac == "oops"){
        std::cout << "ini reader: Parse dst_mac error\n";
        return false;
    }
    else
        dst_mac = temp_dst_mac;

    int temp_dst_port = reader.GetInteger("Corsair", "dst_port", -1);
    if(temp_dst_port == -1){
        std::cout << "ini reader: Parse dst_port error\n";
        return false;
    }
    else
        dst_port = temp_dst_port;


    return true;
}

void Corsair::select_tvgray_chanel()
{
    _CmdManager->select_channel(CORSAIR_CHANNEL_ID_TVGRAY);
    _currChanel = CORSAIR_CHANNEL_ID_TVGRAY;
}

void Corsair::select_tvrgb_chanel()
{
    _CmdManager->select_channel(CORSAIR_CHANNEL_ID_TVRGB);
    _currChanel = CORSAIR_CHANNEL_ID_TVRGB;
}

void Corsair::zoom_in()
{
    _CmdManager->zoom_in(_currChanel);
    zoom_started = true;
}

void Corsair::zoom_out()
{
    _CmdManager->zoom_out(_currChanel);
    zoom_started = true;
}

void Corsair::focus_near()
{
    _CmdManager->focus_near(_currChanel);
    focus_started = true;
}

void Corsair::focus_far()
{
    _CmdManager->focus_far(_currChanel);
    focus_started = true;
}
