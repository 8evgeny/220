#include "gstcv_srt_client.hpp"
#include <iostream>

using namespace gstcv;

SRTClient::SRTClient(const SRTClientCfg &_cfg, bool &ok) : cfg(_cfg)
{
    // std::string pipe = "srtsrc uri=\"" + cfg.srv_uri + "\" "                    // maybe srtclient
    // "! decodebin "
    // "! videoconvert "
    // "! appsink max-buffers=" + std::to_string(cfg.max_buffers) + " drop=True";
    // std::cout << "NOTE: begin create pipeline = \"" + pipe + "\"" << std::endl;
    
    std::string pipe = "srtsrc uri=\"" + cfg.srv_uri + "\" " // or srtclientsrc
    "! tsdemux "
    "! h264parse "
    // "! video/x-h264 "
    "! avdec_h264 "
    "! videoconvert "
    "! appsink max-buffers=" + std::to_string(cfg.max_buffers) + " drop=True";
    
    std::cout << "NOTE: begin create pipeline = \"" + pipe + "\"" << std::endl;
    
    cap = cv::VideoCapture(pipe, cv::CAP_GSTREAMER);
    ok = cap.isOpened();
    if(!ok)
    {
        std::cout << "ERROR: open gstreamer client failed" << std::endl;
    }
} // SRTClient(const SRTClientCfg &_cfg) 

bool SRTClient::getFrame(cv::Mat &frame_rcv)
{
    return cap.read(frame_rcv);
} // -- END getFrame

void SRTClient::quit()
{
    if(sync.quit_was_call.load())
    {
        return;
    }
    sync.quit_was_call.store(true);
    cap.release();
} // -- END quit

SRTClient::~SRTClient()
{
    quit();
} // -- END ~SRTClient