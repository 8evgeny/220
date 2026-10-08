#include "colibritv.h"

#include <opencv2/imgproc.hpp>

ColibriTV::ColibriTV(const std::string &path_to_settings,
                     bool &ok)
{
    ok = false;
    if(getSettings(path_to_settings)){
        _ethController = &ColibriTV_EthController::inst();
        ok = true;
    }
    else
        delete this;
}

bool ColibriTV::getSettings(const std::string &path_to_ini)
{
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){
        std::cout << "ini reader: Parse error\n";
        return false;
    }

    std::string temp_output_format = reader.Get("ColibriTV", "output_format", "oops");
    if(temp_output_format == "oops"){
        std::cout << "ini reader: Parse output_format error\n";
        return false;
    }
    else
        output_format = temp_output_format;

    std::string temp_dev = reader.Get("ColibriTV", "lan_card", "oops");
    if(temp_dev == "oops"){
        std::cout << "ini reader: Parse lan_card error\n";
        return false;
    }
    else
        dev = temp_dev;

    std::string temp_src_mac = reader.Get("ColibriTV", "src_mac", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_mac error\n";
        return false;
    }
    else
        src_mac = temp_src_mac;

    std::string temp_src_ip = reader.Get("ColibriTV", "src_ip", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_ip error\n";
        return false;
    }
    else
        src_ip = temp_src_ip;

    int temp_src_port = reader.GetInteger("ColibriTV", "src_port", -1);
    if(temp_src_port == -1){
        std::cout << "ini reader: Parse src_port error\n";
        return false;
    }
    else
        src_port = temp_src_port;

    std::string temp_dst_ip = reader.Get("ColibriTV", "dst_ip", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse dst_ip error\n";
        return false;
    }
    else
        dst_ip = temp_dst_ip;

    std::string temp_dst_mac = reader.Get("ColibriTV", "dst_mac", "oops");
    if(temp_dst_mac == "oops"){
        std::cout << "ini reader: Parse dst_mac error\n";
        return false;
    }
    else
        dst_mac = temp_dst_mac;

    int temp_dst_port = reader.GetInteger("ColibriTV", "dst_port", -1);
    if(temp_dst_port == -1){
        std::cout << "ini reader: Parse dst_port error\n";
        return false;
    }
    else
        dst_port = temp_dst_port;

    return true;
}

void ColibriTV::workflow()
{
    return void();
}

void ColibriTV::keyHandler(char &key)
{
    return void();
}

int ColibriTV::getColorChannels()
{
    return 1;
}

bool ColibriTV::isBayerColorChannel()
{
    return false;
}

void ColibriTV::getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
{
    image =  cv::Mat(cv::Size(w, h), CV_8UC1, f).clone();
    if(output_format == "BAYER")
    {
        cv::cvtColor(image, image, cv::COLOR_BayerBG2BGR);
    }
}

void ColibriTV::register_frame_handler(IFrameHandler *handler)
{
    _ethController->getServer()->register_frame_handler(handler);
}

void ColibriTV::quit()
{
    _ethController->getServer()->quit();
}

