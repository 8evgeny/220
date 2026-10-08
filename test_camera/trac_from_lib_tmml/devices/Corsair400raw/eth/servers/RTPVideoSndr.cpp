#include "RTPVideoSndr.hpp"
#include "eth/EthGlobal.hpp"
/***************************************************************************/
//    _gstParametrs = "appsrc name=source is-live=true format=GST_FORMAT_BYTES " \
//        "caps=video/x-raw,format=I420,width=1920,height=1080,framerate=25/1 ! "\
//        "nvvideoconvert flip-method=2 ! video/x-raw(memory:NVMM),format=I420 " \
//        "! nvv4l2h265enc control-rate=1 bitrate=6000000 ! h265parse config_interval=-1 ! rtph265pay " \
//        "! udpsink host=127.0.0.1 port=31990 async=false sync=false";
/***************************************************************************/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#include <opencv2/videoio.hpp>
#include "opencv2/core/mat.hpp"
#pragma GCC diagnostic pop

RTPVideoSndr::RTPVideoSndr(const int32_t& width,   const int32_t& height,    const int32_t& fps, bool isColour,
                           const int32_t& bitrate, const std::string& ipDst, const int32_t& port)
    : _fourcc{0}
    , _fps{fps}
    , _isColour{isColour}
    , _bitrate{bitrate}
    , _ipDst{ipDst}
    , _port{port}
    , _frameSize{width, height}
    , _vidWr{new cv::VideoWriter}
    , _gstParametrs{""}
    , _errorOpenCnt{0}
{

    _gstParametrs  = "appsrc is-live=true format=GST_FORMAT_BYTES ";
    _gstParametrs += std::string("caps=video/x-raw,format=I420,") +
                     std::string("width=")      + std::to_string(width) +
                     std::string(",height=")    + std::to_string(height) +
                     std::string(",framerate=") + std::to_string(fps) + "/1 ";
#ifdef NVJETSON
    _gstParametrs +=std::string("! nvvideoconvert flip-method=2 ") +
                    std::string("! video/x-raw(memory:NVMM),format=I420 ") +
                    std::string("! nvv4l2h265enc control-rate=1 ");
    _gstParametrs +="bitrate=" + std::to_string(_bitrate) + " ";
    _gstParametrs +="! h265parse config_interval=-1 ! rtph265pay ";
#else
    _gstParametrs +=std::string("! videoconvert ") +
                     std::string("! video/x-raw,format=I420 ") +
                     std::string("! x264enc ");
    _gstParametrs +="bitrate=" + std::to_string(_bitrate) + " ";
    _gstParametrs +="! h264parse config_interval=-1 ! rtph264pay ";
#endif
    _gstParametrs +="! udpsink host=" + _ipDst +
                     " port=" + std::to_string(_port) +
                     " async=false sync=false";


    ERROR_MSG(_gstParametrs);
//    _gstParametrs = "appsrc name=source is-live=true format=GST_FORMAT_BYTES " \
//        "caps=video/x-raw,format=I420,width=1920,height=1080,framerate=25/1 "\
//        "! nvvideoconvert flip-method=2 ! video/x-raw(memory:NVMM),format=I420 " \
//        "! nvv4l2h265enc control-rate=1 bitrate=6000000 ! h265parse config_interval=-1 ! rtph265pay " \
//        "! udpsink host=127.0.0.1 port=31990 async=false sync=false";

    if(!_vidWr) {
        ERROR_MSG("RTPVideoSndr: VideoWriter not created")
    }
}

RTPVideoSndr::~RTPVideoSndr()
{
    delete _vidWr;
}

void RTPVideoSndr::setParametrs(const std::string& parametrs)
{
    _gstParametrs = parametrs;
    LOG_MSG("RTPVideoSndr: set parametrs", " ");
    LOG_MSG(_gstParametrs, "\n");
}

void RTPVideoSndr::open(const std::string& parametrs)
{
    _gstParametrs = parametrs;
    open();
}

void RTPVideoSndr::open()
{
//    bool result = false;
//    result =
        openStream();
//    return result;
}

void RTPVideoSndr::close()
{
    closeStream();
}

void RTPVideoSndr::setMat(const cv::Mat& frame)
{
    if (_vidWr->isOpened())
    {
        _vidWr->write(frame);
        LOG_MSG("RTPVideoSndr: Frame add №", "");
        LOG_MSG(_frameCounter, "\n");
    }
}

bool RTPVideoSndr::isConnected()
{
    return _vidWr->isOpened();
}

bool RTPVideoSndr::openStream()
{
    bool result = false;
    _vidWr->open(_gstParametrs, cv::CAP_GSTREAMER, _fourcc, _fps, _frameSize, _isColour);
    if (_vidWr->isOpened())
    {
        result = true;
        _errorOpenCnt = 0;
        LOG_MSG("RTPVideoSndr: Stream opened with parametrs", " ");
        LOG_MSG(_gstParametrs, "\n");
        _frameCounter = 0;
    } else {
        ERROR_MSG("RTPVideoSndr: Stream not opened")
        _errorOpenCnt++;
    }
    return result;
}

void RTPVideoSndr::closeStream()
{
    if(_vidWr->isOpened()) {
        _vidWr->release();
        LOG_MSG("RTPVideoSndr: Stream closed", "\n");
    }
}

