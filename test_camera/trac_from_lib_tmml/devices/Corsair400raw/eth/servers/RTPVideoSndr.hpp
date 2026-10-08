#ifndef RTPVIDEOSNDR_HPP
#define RTPVIDEOSNDR_HPP

#include "opencv2/core/types.hpp"
#include <string>
#include <mutex>

namespace cv {
    class VideoWriter ;
    class Mat;
}
class SocketAddress;

class RTPVideoSndr
{
public:
    /**
     *  Constructor
     */
    explicit RTPVideoSndr(const int32_t& width, const int32_t& height, const int32_t& fps, bool isColour,
                          const int32_t& bitrate, const std::string& ipDst, const int32_t& port);
    /**
     *  Destructor
     */
    ~RTPVideoSndr();
    /** Copy and assignment not allowed */
    RTPVideoSndr(const RTPVideoSndr& ) = delete;
    void operator=(const RTPVideoSndr& ) = delete;
    /**
     *  Set gstreamer parametrs
     *
     *  @param parametrs - string with gstreamer parametrs
     */
    void setParametrs(const std::string& parametrs);
    /**
     *  Create cv::VideoCapture(), open stream from gstreamer
     *  and start server thread.
     *
     *  @param parametrs - string with gstreamer parametrs
     */
    void open(const std::string& parametrs);
    /**
     *  Create cv::VideoCapture(), open stream from gstreamer
     *  and start server thread.
     *
     *  @return true     - if successful
     */
    void open();
    /**
     *  Stop server thread and close socket.
     */
    void close();
    /**
     *  Set frame
     */
    void setMat(const cv::Mat& frame);
    /**
     *  Get connection status.
     *
     *  @return true - if gstreamer opening successful and thread running
     */
    bool isConnected();

private:
    /**
     *  Open receive stream from _gstParametrs
     */
    bool openStream();
    /**
     *  Close stream
     */
    void closeStream();

private:
    int32_t    _fourcc;
    int32_t    _fps;
    bool       _isColour;
    int32_t     _bitrate;
    std::string _ipDst;
    int32_t _port;

    int32_t    _frameCounter;
    cv::Size2i _frameSize;
    /** Frame receiver mutex */
    std::mutex _mutex;
    /** Video stream capture */
    cv::VideoWriter* _vidWr;
    /** Stream parametrs for gstreamer */
    std::string _gstParametrs;
    /** Stream opening error counter */
    int32_t _errorOpenCnt;
};

#endif // RTPVIDEOSNDR_HPP
