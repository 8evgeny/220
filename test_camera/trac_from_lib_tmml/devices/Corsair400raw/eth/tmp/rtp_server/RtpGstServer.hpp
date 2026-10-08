#ifndef RTPGSTSERVER_H
#define RTPGSTSERVER_H

#include <memory>
#include <condition_variable>

//#include <gst/gst.h>

#include "RtpGstServerSettings.hpp"

/*
gst-launch-1.0 -e appsrc 
! nvv4l2h264enc bitrate=8000000 insert-sps-pps=true 
! rtph264pay mtu=9000 
! udpsink host=$CLIENT_IP port=5000 sync=false async=false
*/
struct _GstElement;
typedef struct _GstElement GstElement;

namespace rtp
{
    class RtpServer
    {
        struct Synchronisation
        {

        };
    public:
        RtpServer();
        ~RtpServer();
        bool start();
        bool stop();
        void newFrame(uint8_t *buffer, size_t size);
        bool isConnected() const {return _running;}
        std::shared_ptr<rtp::server::Settings> getSettings() {return _settings;}

    private:
        friend void waitData(GstElement *appsrc, uint32_t unused, RtpServer *server);
        void workflow();
        bool setup();

    private:
        std::shared_ptr<rtp::server::Settings> _settings;
        GstElement                            *_pipeline;
        GstElement                            *_appsrc;
        std::mutex _mutex;
        bool _running;
    };
}

#endif // RTPGSTSERVER_H
