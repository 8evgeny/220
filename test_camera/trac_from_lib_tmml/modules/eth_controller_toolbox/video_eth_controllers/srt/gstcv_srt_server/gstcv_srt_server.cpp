#include "gstcv_srt_server.hpp"
#include <iostream>

using namespace gstcv;

SRTServer::SRTServer(const SRTServerCfg &_cfg, bool &ok) : cfg(_cfg)
{
    // properties reference 
    std::string pipe = "appsrc " 
    "! videoconvert "
    "! video/x-raw, height=" + std::to_string(cfg.frame_size.height) + ", width=" + std::to_string(cfg.frame_size.width) + " "
    "! videoconvert "
    // "! x264enc bitrate=32000 key-int-max=3 tune=zerolatency speed-preset=ultrafast byte-stream=true threads=0 " // bitrate=8192
    "! x264enc tune=zerolatency speed-preset=ultrafast "
    "! video/x-h264, profile=baseline " // profile [high, baseline, etc...]
    "! mpegtsmux "
    "! srtserversink uri=srt://:8888 latency=0 "    // srtsink or srtserversink //latency=100
    "wait-for-connection=false "; 

    std::cout << "NOTE: begin create pipeline = \"" + pipe + "\"" << std::endl;


    writer = cv::VideoWriter(
        pipe, 
        cv::CAP_GSTREAMER, 
        0, 
        cfg.fps, 
        cfg.frame_size, 
        cfg.is_color);
    ok = writer.isOpened();

    if(!ok)
    {
        std::cout << "ERROR: open gstreamer server failed" << std::endl;
    }

} // -- END SRTServer(const SRTServerCfg &_cfg, bool &ok)

bool SRTServer::sendFrame(cv::Mat &frame_snd)
{
    writer.write(frame_snd);
    return true;
} // -- END sendFrame

void SRTServer::quit()
{
    if(sync.quit_was_call.load())
    {
        return;
    }
    sync.quit_was_call.store(true);
    writer.release();
} // -- END quit

SRTServer::~SRTServer()
{
    quit();
} // -- END ~SRTServer