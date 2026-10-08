#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <opencv2/core.hpp>

#include <iostream>
#include <string>
#include "icommandhandler.h"
#include "iframehandler.h"

#ifdef USE_ETHERNET_CONNECTION
#include "../eth/ethcontroller.h"
#include "../eth/server.h"
#include "../cmd/cmdmanager.h"
#endif

class Device
{
public:
    Device(){}
    virtual ~Device(){}
    virtual void setup(); // установка соединения с камерой
    virtual void start(){device_error("start");} // установка соединения с камерой
    virtual void quit(){device_error("quit");}
    // получение последнего счатанного кадра
#ifdef USE_ETHERNET_CONNECTION
    CmdManager *getCmdManager();
    void register_command_handler(ICommandHandler * handler);
#endif
    virtual int getFrameCounter()
    {
        return 0;
    }

    virtual int getColorChannels() = 0;
    virtual bool isBayerColorChannel() = 0;

    virtual uint8_t *receiveFrame(int &w, int &h, int &id, int &num);
    virtual bool sendFrame(const uint8_t * f, int w, int h, int id, int num);
    virtual void getFrame(cv::Mat &frame){device_error("get frame");}
    virtual bool getFirstFrame(cv::Mat &frame){getFrame(frame); return true;}
    virtual bool getState() = 0;
    virtual float getExposureMs() {return -1;}

    virtual void keyHandler(unsigned char & key) = 0;
    virtual void workflow() = 0;
    virtual void updateAimPosition(std::pair<float, float> pos){device_error("update aim position");}

    virtual void select_tvgray_chanel(){device_error("Select gray chanel");}
    virtual void select_tvrgb_chanel(){device_error("Select RGB chanel");}
    virtual void el_arr_on(){device_error("el_arr_on");}
    virtual void stabilisation_on(){device_error("stabilisation_on");}
    virtual void set_speed(const int32_t & _x_speed, const int32_t & _z_speed){device_error("set_speed");}
    virtual void zoom_in(){device_error("zoom_tv_plus");}
    virtual void zoom_out(){device_error("zoom_tv_plus");}
    virtual void focus_near(){device_error("focus_near");}
    virtual void focus_far(){device_error("focus_far");}

    virtual void ping(){device_error("ping");}
    virtual void send_target_capture(const int & id, const bool & fast,
                                     const float & x, const float & y,
                                     const float & w, const float & h)
    {
        device_error("send_target_capture");
    }

    virtual void send_target_capture_with_roi(const int & id, const bool & fast,
                                              const float & x, const float & y,
                                              const float & w, const float & h,
                                              const float & x_roi, const float & y_roi,
                                              const float & w_roi, const float & h_roi)
    {
        device_error("send_target_capture_with_roi");
    }
    virtual void send_target_free(const int & id, const int &target_id)
    {
        device_error("send_target_capture");
    }
    virtual void set_tracker_enable(const bool enable, const int id)
    {
        device_error("tracker_enable");
    }

    virtual void register_frame_handler(IFrameHandler *handler)
    {
        frame_handlers.push_back(handler);
    }

    virtual void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
    {
        device_error("getFormatedImage");
    }

    virtual int getId() {return frame_id;}

protected:
#ifdef USE_ETHERNET_CONNECTION
    std::string dev = "eno1";
    std::string src_mac  = "ff-ff-ff-ff-ff-ff";
    std::string src_ip   = "169.254.0.2";
    uint16_t    src_port = 52515;
    std::string dst_mac  = "00-40-d0-21-33-c7";
    std::string dst_ip   = "169.254.0.1";
    uint16_t    dst_port = 52516;

    EthController* _ethController = nullptr;
    CmdManager* _CmdManager = nullptr;
#endif
    int frame_id = 0;
    std::vector<ICommandHandler *> cmd_handlers;
    std::vector<IFrameHandler *> frame_handlers;

    int colorChannelsNum = 1;
    bool isBayerColor = false;

    void device_error(const std::string &cmd){
        std::cout << "No command " << cmd << " for this device" << std::endl;
    }
    virtual bool getSettings(const std::string& path_to_ini){return false;}
};

#endif // DEVICE_HPP
