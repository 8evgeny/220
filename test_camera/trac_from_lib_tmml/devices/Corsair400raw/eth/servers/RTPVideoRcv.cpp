#include "RTPVideoRcv.hpp"
#include "eth/EthGlobal.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#include "opencv2/videoio.hpp"
#pragma GCC diagnostic pop

#include <iostream>
//const char *gst = "videotestsrc pattern=checkers-8 ! video/x-raw,framerate=25/1 ! videoscale ! videoconvert ! appsink";
//    const char *gst = "rtspsrc location=rtsp://username:password@ip:port/streamPath ! rtph264depay ! h264parse ! omxh264dec ! videoconvert ! appsink";
//const char *gst = ;
//const char *gst = "udpsrc port=52517 ! application/x-rtp,media=video,payload=96,clock-rate=90000,encoding-name=H265 ! rtph265depay ! h265parse config_interval=-1 ! decodebin ! videoconvert  ! appsink";
// gst-launch-1.0.exe --gst-debug=2 udpsrc port=52517 caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H265, payload=(int)96" ! rtph265depay ! h265parse config_interval=-1 ! decodebin ! fpsdisplaysink sync=false
// gst-launch-1.0.exe --gst-debug=2 udpsrc port=52517 ! application/x-rtp, media=video, clock-rate=90000, encoding-name=H265, payload=96 ! rtph265depay ! h265parse config_interval=-1 ! decodebin ! appsink"
//    VideoCapture cap("udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink",  CAP_GSTREAMER);


RTPVideoRcv::RTPVideoRcv()
    : BaseThread(10)
    ,  _cap{new cv::VideoCapture}
    , _it{0}
    , _itView{_it}
    , _frameBufferNum{2}
    , _gstParametrs{"udpsrc port=52517 ! application/x-rtp,media=video,payload=96,clock-rate=90000,encoding-name=H265 ! rtph265depay ! h265parse config_interval=-1 ! decodebin ! videoconvert  ! appsink"}
    , _errorOpenCnt{0}
{
    for (int var = 0; var < _frameBufferNum; ++var) {
        _frameBuffer.push_back(cv::Mat());
    }

    if(!_cap) {
        ERROR_MSG("RTPVideoRcv: VideoCapture not created");
    }
}

RTPVideoRcv::~RTPVideoRcv()
{
    _frameBuffer.clear();
    delete _cap;
}

void RTPVideoRcv::setParametrs(const std::string& parametrs)
{
    _gstParametrs = parametrs;
}

void RTPVideoRcv::open(const std::string& parametrs)
{
    _gstParametrs = parametrs;
    open();
}

void RTPVideoRcv::open()
{
    startThd();
}

void RTPVideoRcv::close()
{
    if(isRunningThd()) {
        BaseThread::stopThd();
    }
}

const cv::Mat& RTPVideoRcv::getMat()
{
    std::lock_guard<std::mutex>  lock(_mutex);
    return _frameBuffer.at(_itView);
}

bool RTPVideoRcv::isConnected()
{
    return isRunningThd() && _cap->isOpened();
}

bool RTPVideoRcv::initThd()
{
    bool result = false;
    result = openStream();
    return result;
}

void RTPVideoRcv::runThd()
{
    if (_cap->isOpened()) {
        *_cap >> _frameBuffer.at(_it);
        std::lock_guard<std::mutex>  lock(_mutex);
        _itView = _it;
        _it++;
        if(!(_it < _frameBufferNum)) {
            _it = 0;
        }
    } else {
        openStream();
        if(_errorOpenCnt > 3) {
            setExecuteFlagThd(false);
        }
    }
}

void RTPVideoRcv::closeThd()
{
    closeStream();
}

bool RTPVideoRcv::openStream()
{
    bool result = false;
    _cap->open(_gstParametrs.c_str(), cv::CAP_GSTREAMER);
    if (_cap->isOpened())
    {
        result = true;
        _errorOpenCnt = 0;
    } else {
        ERROR_MSG("RTPVideoRcv: VideoCapture not opened")
        _errorOpenCnt++;
    }
    return result;
}

void RTPVideoRcv::closeStream()
{
    if(_cap->isOpened()) {
        _cap->release();
    }
}
