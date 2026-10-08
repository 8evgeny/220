#ifndef RTPVIDEOSERVER_H
#define RTPVIDEOSERVER_H

#include "eth/base/BaseThread.hpp"
#include "opencv2/core/mat.hpp"

namespace cv {
    class VideoCapture;
}
class SocketAddress;

class RTPVideoServer : public BaseThread
{
public:
    /**
     *  Constructor
     */
    explicit RTPVideoServer();
    /**
     *  Destructor
     */
    ~RTPVideoServer();
    /** Copy and assignment not allowed */
    RTPVideoServer(const RTPVideoServer& ) = delete;
    void operator=(const RTPVideoServer& ) = delete;
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
     *  Get frame
     *
     *  @return frame
     */
    const cv::Mat& getMat();
    /**
     *  Get connection status.
     *
     *  @return true - if gstreamer opening successful and thread running
     */
    bool isConnected();

protected:
    /**
     *  Initialization server data before thread loop
     *  Include in "BaseThread::run()"
     *
     *  @return true - if successful
     */
    bool initThd() override;
    /**
     *  Server thread loop.
     *  Include in "BaseThread::run()"
     */
    void runThd() override;
    /**
     *  Deinit server data after leaving the thread loop.
     *  Include in "BaseThread::run()"
     */
    void closeThd() override;

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
    /** Frame receiver mutex */
    std::mutex _mutex;
    /** Video stream capture */
    cv::VideoCapture* _cap;
    /** Iterater on active frame buffer */
    int32_t _it;
    /** Iterater on complete frame buffer */
    int32_t _itView;
    /** Number frame buffer */
    const int32_t _frameBufferNum;
    /** Frame buffer */
    std::vector<cv::Mat> _frameBuffer;
    /** Stream parametrs for gstreamer */
    std::string _gstParametrs;
    /** Stream opening error counter */
    int32_t _errorOpenCnt;

};

#endif // RTPVIDEOSERVER_H
